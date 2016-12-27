#include "include/vkalloc.h"

#include <vector>
#include <unordered_map>
#include <mutex>

#ifndef VKA_ALLOC_SIZE
#define VKA_ALLOC_SIZE 1024*1024*32
#endif

namespace vka {
    struct Node {
        Node* next = nullptr;
        uint64_t offset = 0;
        uint64_t size = 0;
    };

    struct Page {
        VkDeviceMemory deviceMemory = VK_NULL_HANDLE;
        Node* head = nullptr;
    };

    struct Heap {
        std::vector<Page> pages;
        std::mutex mutex;
    };

    struct PageMapping {
        uint32_t heapIndex;
        uint32_t pageIndex;
    };

    VkDevice device;
    VkPhysicalDeviceMemoryProperties properties;
    VkAllocationCallbacks* callbacks;
    std::unordered_map<VkDeviceMemory, PageMapping> pageMap;    //caches index into one of the vector<Page> heaps
    Heap* heaps;
}

void vkaInit(VkPhysicalDevice physicalDevice, VkDevice device, VkAllocationCallbacks* allocator){
    vka::device = device;
    vka::callbacks = allocator;

    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &vka::properties);

    vka::heaps = new vka::Heap[vka::properties.memoryHeapCount];
}

using namespace vka;
static VkAllocation AttemptAlloc(uint32_t typeIndex, uint32_t heapIndex, VkMemoryRequirements requirements);
static VkAllocation AttemptAlloc(Page& page, VkMemoryRequirements requirements);
static Page* AllocNewPage(std::vector<Page>& heap, uint32_t typeIndex, uint32_t heapIndex, VkMemoryRequirements requirements);
static void Split(Node* current, Node** last, uint64_t start, uint64_t size);
static void Free(VkAllocation allocation, Page& page);
static void Merge(VkAllocation allocation, Node* current);

void vkaTerminate(){
    pageMap.clear();

    for (size_t i = 0; i < properties.memoryHeapCount; i++) {
        Heap& heap = heaps[i];
        for (auto& page : heap.pages) {
            Node* current = page.head;

            while (current){
                Node* next = current->next;
                delete current;
                current = next;
            }

            vkFreeMemory(device, page.deviceMemory, callbacks);
        }

        heap.pages.clear();
    }

    delete heaps;
}

VkAllocation vkaAlloc(VkMemoryRequirements requirements, VkMemoryPropertyFlags flags) {
    for (size_t i = 0; i < properties.memoryTypeCount; i++) {
        if ((requirements.memoryTypeBits & (1 << i))
            && (properties.memoryTypes[i].propertyFlags & flags) == flags) {
            uint32_t heapIndex = properties.memoryTypes[i].heapIndex;

            VkAllocation result = AttemptAlloc(static_cast<uint32_t>(i), heapIndex, requirements);
            if (result.deviceMemory != VK_NULL_HANDLE) {
                return result;
            }
        }
    }

    return {};
}

void vkaFree(VkAllocation allocation){
    if (allocation.deviceMemory == VK_NULL_HANDLE) return;

    PageMapping& mapping = pageMap[allocation.deviceMemory];
    Page& page = heaps[mapping.heapIndex].pages[mapping.pageIndex];

    Free(allocation, page);
}

static VkAllocation AttemptAlloc(uint32_t typeIndex, uint32_t heapIndex, VkMemoryRequirements requirements) {
    Heap& heap = heaps[heapIndex];
    std::vector<Page>& pages = heap.pages;

    heap.mutex.lock();

    //attempt to allocate from existing pages
    for (Page& page : pages) {
        VkAllocation result = AttemptAlloc(page, requirements);
        if (result.deviceMemory != VK_NULL_HANDLE) {
            heap.mutex.unlock();
            return result;
        }
    }

    //attempt to allocate from new page
    Page* newPage = AllocNewPage(pages, typeIndex, heapIndex, requirements);
    if (newPage) {
        VkAllocation result = AttemptAlloc(*newPage, requirements);
        if (result.deviceMemory != VK_NULL_HANDLE) {
            heap.mutex.unlock();
            return result;
        }
    }

    //give up
    heap.mutex.unlock();
    return {};
}

static VkAllocation AttemptAlloc(Page& page, VkMemoryRequirements requirements) {
    Node* current = page.head;
    Node** last = &page.head;

    while (current) {
        uint64_t start = current->offset;
        uint64_t available = current->size;

        uint64_t unalign = (start % requirements.alignment);
        uint64_t align;

        if (unalign == 0) {
            align = 0;
        } else {
            align = requirements.alignment - unalign;
        }

        start += align;
        available -= align;

        if (available >= requirements.size) {
            Split(current, last, start, requirements.size);
            VkAllocation result;
            result.deviceMemory = page.deviceMemory;
            result.offset = start;
            result.size = requirements.size;
            return result;
        }

        last = &(current->next);
        current = current->next;
    }

    return {};
}

static void Split(Node* current, Node** last, uint64_t start, uint64_t size) {
    uint64_t startSpace = start - current->offset;
    uint64_t endSpace = current->size - (start + size);

    if (startSpace == 0 && endSpace == 0) {
        *last = current->next;
        delete current;
    } else if (startSpace == 0 && endSpace > 0) {
        current->offset = start + size;
        current->size = endSpace;
    } else if (startSpace > 0 && endSpace == 0) {
        current->size = startSpace;
    } else {    //startSpace > 0 && endSpace > 0
        Node* newNode = new Node;
        newNode->next = current->next;
        newNode->offset = start + size;
        newNode->size = endSpace;

        current->next = newNode;
        current->size = startSpace;
    }
}

static Page* AllocNewPage(std::vector<Page>& heap, uint32_t typeIndex, uint32_t heapIndex, VkMemoryRequirements requirements) {
    uint64_t allocSize = VKA_ALLOC_SIZE;
    if (requirements.size > allocSize) {
        allocSize = requirements.size;
    }

    VkMemoryAllocateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    info.allocationSize = allocSize;
    info.memoryTypeIndex = typeIndex;

    VkDeviceMemory memory;
    VkResult result = vkAllocateMemory(device, &info, callbacks, &memory);

    if (result == VK_SUCCESS){
        Node* node = new Node;
        node->size = allocSize;

        Page page = {};
        page.deviceMemory = memory;
        page.head = node;

        heap.emplace_back(page);
        uint32_t pageIndex = static_cast<uint32_t>(heap.size() - 1);

        pageMap[memory] = {heapIndex, pageIndex};

        return &(heap[pageIndex]);
    } else {
        //TODO: add way for user to check the error
        return nullptr;
    }
}

static void Free(VkAllocation allocation, Page& page) {
    Node* current = page.head;

    while (current) {
        if (allocation.offset > current->offset && allocation.offset < current->next->offset) {
            Merge(allocation, current);
            return;
        }

        current = current->next;
    }
}

static void Merge(VkAllocation allocation, Node* current) {
    if (allocation.offset == current->offset + current->size) {
        current->size += allocation.size;
    } else {
        Node* newNode = new Node;
        newNode->next = current->next,
        newNode->offset = allocation.offset,
        newNode->size = allocation.size;

        current = newNode;
    }

    Node* next = current->next;
    if (next->offset == current->offset + current->size) {
        current->size += next->size;
        current->next = next->next;
        delete next;
    }
}
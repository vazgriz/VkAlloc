#include "include/vkalloc.h"

#include <vector>
#include <unordered_map>

namespace vka {
    struct Page {
        VkDeviceMemory deviceMemory = VK_NULL_HANDLE;
        Node* head = nullptr;
    };

    struct Node {
        Node* next = nullptr;
        uint64_t offset = 0;
        uint64_t size = 0;
    };

    VkPhysicalDevice physicalDevice;
    VkDevice device;
    VkPhysicalDeviceMemoryProperties properties;
    VkAllocationCallbacks* callbacks;
    std::unordered_map<VkDeviceMemory, int32_t> pageMap;    //caches index into one of the vector<Page> heaps
    std::vector<Page> heaps[32];    //32 possible heaps
}

void vkaInit(VkPhysicalDevice physicalDevice, VkDevice device, VkAllocationCallbacks* allocator){
    vka::physicalDevice = physicalDevice;
    vka::device = device;
    vka::callbacks = allocator;

    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &vka::properties);
}

using namespace vka;
static VkAllocation AttemptAlloc(std::vector<Page>& heap, uint32_t heapIndex, VkMemoryRequirements requirements);
static VkAllocation AttemptAlloc(Page& page, VkMemoryRequirements requirements);
static Page* AllocNewPage(std::vector<Page>& heap, uint32_t heapIndex, VkMemoryRequirements requirements);

void vkaTerminate(){
    pageMap.clear();

    for (size_t i = 0; i < 32; i++) {
        for (size_t j = 0; j < heaps[i].size(); j++) {
            Node* current = heaps[i][j].head;

            while (current){
                Node* next = current->next;
                delete current;
                current = next;
            }
        }

        heaps[i].clear();
    }
}

VkAllocation vkAlloc(VkMemoryRequirements requirements){
    for (size_t i = 0; i < properties.memoryTypeCount; i++) {
        if ((requirements.memoryTypeBits & (1 << i))
            && properties.memoryTypes[i].propertyFlags == VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) {
            std::vector<Page> &heap = heaps[i];

            VkAllocation result = AttemptAlloc(heap, i, requirements);
            if (result.deviceMemory != VK_NULL_HANDLE) {
                return result;
            }
        }
    }

    return {};
}

void vkFree(VkAllocation allocation){

}

VkAllocation vkHostAlloc(VkMemoryRequirements requirements){

}

void vkHostFree(VkAllocation allocation){

}

static VkAllocation AttemptAlloc(std::vector<Page>& heap, uint32_t heapIndex, VkMemoryRequirements requirements) {
    //attempt to allocate from existing pages
    for (size_t i = 0; i < heap.size(); i++) {
        Page& page = heap[i];

        VkAllocation result = AttemptAlloc(page, requirements);
        if (result.deviceMemory != VK_NULL_HANDLE) {
            return result;
        }
    }

    //attempt to allocate from new page
    Page* newPage = AllocNewPage(heap, heapIndex, requirements);
    if (newPage) {
        VkAllocation result = AttemptAlloc(*newPage, requirements);
        if (result.deviceMemory != VK_NULL_HANDLE) {
            return result;
        }
    }

    //give up
    return {};
}

static VkAllocation AttemptAlloc(Page& page, VkMemoryRequirements requirements) {
    Node* current = page.head;

    while (current) {
        uint64_t start = current->offset;
    }
}

static Page* AllocNewPage(std::vector<Page>& heap, uint32_t heapIndex, VkMemoryRequirements requirements) {
    uint64_t allocSize = VKA_ALLOC_SIZE;
    if (requirements.size > allocSize) {
        allocSize = requirements.size;
    }

    VkMemoryAllocateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    info.allocationSize = allocSize;
    info.memoryTypeIndex = heapIndex;

    VkDeviceMemory memory;
    VkResult result = vkAllocateMemory(device, &info, callbacks, &memory);

    if (result == VK_SUCCESS){
        Page page = {};
        page.deviceMemory = memory;

        Node* node = new Node{};
        node->size = allocSize;

        page.head = node;

        heap.emplace_back(page);

        return &(heap[heap.size() - 1]);
    } else {
        //TODO: add way for user to check the error
        return nullptr;
    }
}
#include "include/vkaPage.h"

using namespace vka;

Page::Page(VkDevice device, size_t size, uint32_t typeIndex, std::unordered_map<VkDeviceMemory, Page*>& pageMap, VkAllocationCallbacks* callbacks) {
    this->size = size;
    mutex = new std::mutex();
    this->device = device;
    this->callbacks = callbacks;
    this->typeIndex = typeIndex;

    head = new Node(0, size);

    VkMemoryAllocateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    info.allocationSize = size;
    info.memoryTypeIndex = typeIndex;

    vkAllocateMemory(device, &info, callbacks, &memory);

    pageMap.insert({ memory, this });
}

Page::Page(Page&& other) {
    head = other.head;
    size = other.size;
    mutex = other.mutex;
    device = other.device;
    memory = other.memory;
    callbacks = other.callbacks;

    other.head = nullptr;
    other.mutex = nullptr;
    other.memory = VK_NULL_HANDLE;
}

Page::~Page() {
    vkFreeMemory(device, memory, callbacks);

    Node* current = head;

    while (current) {
        Node* next = current->next;
        delete current;
        current = next;
    }

    delete mutex;
}

bool const Page::Match(uint32_t typeIndex) const {
    return this->typeIndex == typeIndex;
}

VkaAllocation Page::AttemptAlloc(VkMemoryRequirements requirements) {
    if (requirements.size > size) return {};
    std::lock_guard<std::mutex> lock(*mutex);

    Node* current = head;

    while (current) {
        if (current->free && current->size >= requirements.size) {
            size_t start = current->offset;
            size_t available = current->size;

            size_t unalign = start % requirements.alignment;
            size_t align;

            if (unalign == 0) {
                align = 0;
            } else {
                align = requirements.alignment - unalign;
            }

            start += align;
            available -= align;

            if (available >= requirements.size) {
                current->Split(start, requirements.size);
                VkaAllocation result;
                result.memory = memory;
                result.offset = start;
                result.size = requirements.size;
                return result;
            }
        }

        current = current->next;
    }

    return {};
}

void Page::Free(VkaAllocation allocation) {
    std::lock_guard<std::mutex> locker(*mutex);

    Node* current = head;

    while (current) {
        if (current->free && current->offset == allocation.offset && current->size == allocation.size) {
            current->free = true;
            break;
        }
    }

    current = head;

    while (current) {
        current->Merge();
        current = current->next;
    }
}
#include "include/vkaHeap.h"

using namespace vka;

Heap::Heap(uint32_t heapIndex, size_t pageSize, VkPhysicalDeviceMemoryProperties& props, VkDevice device, VkAllocationCallbacks* callbacks, std::unordered_map<VkDeviceMemory, Page*>& pageMap) : pageMap(pageMap) {
    this->heapIndex = heapIndex;
    this->pageSize = pageSize;
    numTypes = props.memoryTypeCount;
    mutex.reset(new std::mutex());
    this->callbacks = callbacks;
    this->device = device;

    for (size_t i = 0; i < props.memoryTypeCount; i++) {
        VkMemoryType& type = props.memoryTypes[i];

        if (type.heapIndex == heapIndex) {
            typeIndices.emplace(static_cast<uint32_t>(i));
            heapFlags.emplace_back(type.propertyFlags);
        }
    }
}

bool const Heap::Match(VkMemoryRequirements requirements, VkMemoryPropertyFlags flags, uint32_t* typeIndex) const {
    bool typeMatch = false;
    bool flagMatch = false;

    for (uint32_t i = 0; i < numTypes; i++) {
        if ((requirements.memoryTypeBits & (1 << i)) != 0 && typeIndices.count(i) > 0) {
            typeMatch = true;
            *typeIndex = i;
            break;
        }
    }
    
    for (size_t i = 0; i < heapFlags.size(); i++) {
        if ((heapFlags[i] & flags) == flags) {
            flagMatch = true;
            break;
        }
    }

    return typeMatch && flagMatch;
}

uint32_t const Heap::GetIndex() const {
    return heapIndex;
}

VkaAllocation Heap::Alloc(VkMemoryRequirements requirements, uint32_t typeIndex) {
    std::lock_guard<std::mutex> lock(*mutex);

    for (size_t i = 0; i < pages.size(); i++) {
        VkaAllocation result = pages[i].AttemptAlloc(requirements);
        if (result.memory != VK_NULL_HANDLE) {
            return result;
        }
    }

    size_t size = pageSize;
    if (requirements.size > size) size = requirements.size;
    pages.emplace_back(device, size, typeIndex, pageMap, callbacks);
    return pages[pages.size() - 1].AttemptAlloc(requirements);
}


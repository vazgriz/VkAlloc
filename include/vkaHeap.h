#ifndef VKALLOC_VKAHEAP_H
#define VKALLOC_VKAHEAP_H

#include <vulkan/vulkan.h>
#include <vector>
#include <set>
#include <mutex>
#include <memory>
#include <unordered_map>

#include "include/vkaAllocation.h"
#include "include/vkaPage.h"

namespace vka {
    class Heap {
    public:
        Heap(uint32_t heapIndex, size_t pageSize, VkPhysicalDeviceMemoryProperties& props, VkDevice device, VkAllocationCallbacks* callbacks, std::unordered_map<VkDeviceMemory, Page*>& pageMap);

        uint32_t const GetIndex() const;
        bool const Match(VkMemoryRequirements requirements, VkMemoryPropertyFlags flags, uint32_t* typeIndex) const;
        VkaAllocation Alloc(VkMemoryRequirements requirements, uint32_t typeIndex);

    private:
        uint32_t heapIndex;
        size_t pageSize;
        VkDevice device;
        VkAllocationCallbacks* callbacks;
        std::unordered_map<VkDeviceMemory, Page*>& pageMap;

        std::vector<uint32_t> typeIndices;
        std::vector<VkMemoryPropertyFlags> heapFlags;
        std::vector<Page> pages;
        std::unique_ptr<std::mutex> mutex;
    };
}

#endif //VKALLOC_VKAHEAP_H

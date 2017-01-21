#ifndef VKALLOC_VKAALLOCATOR_H
#define VKALLOC_VKAALLOCATOR_H

#include <vulkan/vulkan.h>
#include <vector>
#include <unordered_map>
#include <memory>

#include "include/vkaAllocation.h"
#include "include/vkaHeap.h"

namespace vka {
    class Allocator {
    public:
        Allocator(VkPhysicalDevice physicalDevice, VkDevice device, VkAllocationCallbacks* callbacks, size_t pageSize);

        VkaAllocation Alloc(VkMemoryRequirements requirements, VkMemoryPropertyFlags flags);
        void Free(VkaAllocation allocation);

    private:
        VkPhysicalDevice physicalDevice;
        VkDevice device;
        VkAllocationCallbacks* callbacks;

        std::vector<Heap> heaps;   //heap contains a mutex, which can't be moved
        std::unordered_map<VkDeviceMemory, Page*> pageMap;
    };
}

#endif //VKALLOC_VKAALLOCATOR_H

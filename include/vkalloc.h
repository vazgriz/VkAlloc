#ifndef VKALLOC_VKALLOC_H
#define VKALLOC_VKALLOC_H

#include <stdlib.h>
#include <stdint.h>
#include <vulkan/vulkan.h>

struct VkAllocation {
    VkDeviceMemory deviceMemory = VK_NULL_HANDLE;
    uint64_t offset = 0;
    uint64_t size = 0;
};

void vkaInit(VkPhysicalDevice physicalDevice, VkDevice device, VkAllocationCallbacks* allocator);
void vkaTerminate();

VkAllocation vkaAllocFlag(VkMemoryRequirements requirements, VkMemoryPropertyFlags flags);
void vkaFree(VkAllocation allocation);

VkAllocation vkaAlloc(VkMemoryRequirements requirements);
VkAllocation vkaAllocHost(VkMemoryRequirements requirements);

#endif //VKALLOC_VKALLOC_H

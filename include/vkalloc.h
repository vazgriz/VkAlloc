#ifndef VKALLOC_VKALLOC_H
#define VKALLOC_VKALLOC_H

#include <stdlib.h>
#include <stdint.h>
#include <vulkan/vulkan.h>

#ifndef VKA_ALLOC_SIZE
#define VKA_ALLOC_SIZE 1024*1024*4
#endif

struct VkAllocation {
    VkDeviceMemory deviceMemory;
    uint64_t offset;
    uint64_t size;
};

void vkaInit(VkPhysicalDevice physicalDevice, VkDevice device);
void vkaTerminate();

VkAllocation vkAlloc(VkMemoryRequirements requirements);
void vkFree(VkAllocation allocation);

VkAllocation vkHostAlloc(VkMemoryRequirements requirements);
void vkHostFree(VkAllocation allocation);

#endif //VKALLOC_VKALLOC_H

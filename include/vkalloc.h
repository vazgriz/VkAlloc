#ifndef VKALLOC_VKALLOC_H
#define VKALLOC_VKALLOC_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <stdint.h>
#include <vulkan/vulkan.h>

#ifndef VKA_ALLOC_SIZE
#define VKA_ALLOC_SIZE 1024*1024*4
#endif

typedef struct {
    VkDeviceMemory deviceMemory;
    uint64_t offset;
    uint64_t size;
} VkAllocation;

void vkaInit(VkPhysicalDevice physicalDevice, VkDevice device);
void vkaTerminate();

VkAllocation vkAlloc(uint64_t size, uint32_t mask);
void vkFree(VkAllocation allocation);

VkAllocation vkHostAlloc(uint64_t size, uint32_t mask);
void vkHostFree(VkAllocation allocation);

#ifdef __cplusplus
}
#endif

#endif //VKALLOC_VKALLOC_H

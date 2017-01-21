#ifndef VKALLOC_VKAALLOCATION_H
#define VKALLOC_VKAALLOCATION_H

#include <vulkan/vulkan.h>

struct VkaAllocation {
    VkDeviceMemory memory = VK_NULL_HANDLE;
    size_t offset = 0;
    size_t size = 0;
};

#endif //VKALLOC_VKAALLOCATION_H

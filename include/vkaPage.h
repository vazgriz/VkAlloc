#ifndef VKALLOC_VKAPAGE_H
#define VKALLOC_VKAPAGE_H

#include <vulkan/vulkan.h>
#include <mutex>
#include <memory>
#include <unordered_map>

#include "include/vkaAllocation.h"
#include "include/vkaNode.h"

namespace vka {
    class Page {
    public:
        Page(VkDevice device, size_t size, uint32_t typeIndex, std::unordered_map<VkDeviceMemory, Page*>& pageMap, VkAllocationCallbacks* callbacks);
        Page(Page&& other);
        ~Page();

        VkaAllocation AttemptAlloc(VkMemoryRequirements requirements);
        void Free(VkaAllocation allocation);

    private:
        Node* head = nullptr;
        size_t size;
        std::mutex* mutex;
        VkDevice device;
        VkDeviceMemory memory;
        VkAllocationCallbacks* callbacks;
    };
}

#endif //VKALLOC_VKAPAGE_H

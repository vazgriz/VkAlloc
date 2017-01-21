#ifndef VKALLOC_VKAPAGE_H
#define VKALLOC_VKAPAGE_H

#include <vulkan/vulkan.h>
#include <mutex>
#include <memory>

#include "include/vkaAllocation.h"
#include "include/vkaNode.h"

namespace vka {
    class Page {
    public:
        Page(size_t size, uint32_t typeIndex);

        VkaAllocation AttemptAlloc(VkMemoryRequirements requirements);
        void Free(VkaAllocation allocation);

    private:
        std::unique_ptr<Node, void(*)(Node*)> head = nullptr;
        size_t size;
        std::unique_ptr<std::mutex> mutex;
        VkDeviceMemory memory;

        static void Delete(Node* head);
    };
}

#endif //VKALLOC_VKAPAGE_H

#ifndef VKALLOC_VKAPAGE_H
#define VKALLOC_VKAPAGE_H

#include <vulkan/vulkan.h>

#include "include/vkaAllocation.h"
#include "include/vkaNode.h"

namespace vka {
    class Page {
    public:
        Page(size_t size, uint32_t typeIndex);
        ~Page();

        VkaAllocation AttemptAlloc(VkMemoryRequirements requirements);

    private:
        Node* head = nullptr;
        size_t size;
    };
}

#endif //VKALLOC_VKAPAGE_H

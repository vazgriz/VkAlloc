#ifndef VKALLOC_VKANODE_H
#define VKALLOC_VKANODE_H

#include <vulkan/vulkan.h>

namespace vka {
    class Node {
    public:
        Node* next;
        size_t offset;
        size_t size;
        bool free;

        Node(size_t offset, size_t size);
    };
}

#endif //VKALLOC_VKANODE_H

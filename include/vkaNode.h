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

        void Split(size_t start, size_t size);
        void Merge();
    };
}

#endif //VKALLOC_VKANODE_H

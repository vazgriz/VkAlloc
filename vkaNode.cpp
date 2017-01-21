#include "include/vkaNode.h"

using namespace vka;

Node::Node(size_t offset, size_t size) {
    this->offset = offset;
    this->size = size;
    free = true;
}

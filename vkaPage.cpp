#include "include/vkaPage.h"

using namespace vka;

Page::Page(size_t size, uint32_t typeIndex) {
    head = new Node(0, size);
    this->size = size;
}

Page::~Page() {
    Node* current = head;
    while (current) {
        Node* next = current->next;
        delete current;
        current = next;
    }
}

VkaAllocation Page::AttemptAlloc(VkMemoryRequirements requirements) {
    if (requirements.size > size) return {};

    return {};
}
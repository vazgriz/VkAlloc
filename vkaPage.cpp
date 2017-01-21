#include "include/vkaPage.h"

using namespace vka;

Page::Page(size_t size, uint32_t typeIndex) : head(new Node(0, size), Delete) {
    this->size = size;
    mutex.reset(new std::mutex());
}

VkaAllocation Page::AttemptAlloc(VkMemoryRequirements requirements) {
    if (requirements.size > size) return {};
    std::lock_guard<std::mutex> lock(*mutex);

    Node* current = head.get();

    while (current) {
        if (current->free && current->size >= requirements.size) {
            size_t start = current->offset;
            size_t available = current->size;

            size_t unalign = start % requirements.alignment;
            size_t align;

            if (unalign == 0) {
                align = 0;
            } else {
                align = requirements.alignment - unalign;
            }

            start += align;
            available -= align;

            if (available >= requirements.size) {
                current->Split(start, requirements.size);
                VkaAllocation result;
                result.memory = memory;
                result.offset = start;
                result.size = requirements.size;
                return result;
            }
        }

        current = current->next;
    }

    return {};
}

void Page::Free(VkaAllocation allocation) {
    std::lock_guard<std::mutex> locker(*mutex);

    Node* current = head.get();

    while (current) {
        if (current->free && current->offset == allocation.offset && current->size == allocation.size) {
            current->free = true;
            break;
        }
    }

    current = head.get();

    while (current) {
        current->Merge();
        current = current->next;
    }
}

void Page::Delete(Node* head) {
    while (head) {
        Node* next = head->next;
        delete head;
        head = next;
    }
}
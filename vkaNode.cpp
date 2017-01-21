#include "include/vkaNode.h"

using namespace vka;

Node::Node(size_t offset, size_t size) {
    this->offset = offset;
    this->size = size;
    free = true;
}

void Node::Split(size_t start, size_t size) {
    //split a node and mark the correct one as not free
    //this node can potentially be split into three if start and size defines a space in the middle of the node

    if (start == offset && this->size == size) {
        //entire node was taken, so just mark it as free
        free = false;
    } else if (start > offset) {
        //some space was left in the beginning, so use this node for that and mark a new one as not free
        size_t startSpace = start - offset;
        this->size = startSpace;

        Node* middle = new Node(start, size);
        middle->next = next;
        next = middle;

        middle->Split(start, size);
    } else {
        //only some space left at the back
        free = false;
        size_t endOffset = start + size;
        size_t endSpace = (offset + this->size) - endOffset;

        Node* end = new Node(endOffset, endSpace);
        end->next = next;
        next = end;
    }
}

void Node::Merge() {
    if (free) {
        Node* current = this->next;
        while (current != nullptr && current->free) {
            size += current->size;
            this->next = current->next;
            delete current;
        }
    }
}
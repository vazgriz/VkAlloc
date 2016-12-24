#include "include/vkalloc.h"

#include <vector>
#include <unordered_map>

namespace vka {
    struct Page {
        VkDeviceMemory deviceMemory;
        Node* head = nullptr;
    };

    struct Node {
        Node* next = nullptr;
        uint64_t offset;
        uint64_t size;
    };

    VkPhysicalDevice physicalDevice;
    VkDevice device;
    VkPhysicalDeviceMemoryProperties properties;
    std::unordered_map<VkDeviceMemory, int32_t> pageMap;    //caches index into one of the vector<Page> heaps
    std::vector<Page> heaps[32];    //32 possible heaps
}

void vkaInit(VkPhysicalDevice physicalDevice, VkDevice device){
    vka::physicalDevice = physicalDevice;
    vka::device = device;

}

using namespace vka;

void vkaTerminate(){
    pageMap.clear();

    for (size_t i = 0; i < 32; i++) {
        for (size_t j = 0; j < heaps[i].size(); j++) {
            Node* current = heaps[i][j].head;

            while (current){
                Node* next = current->next;
                delete current;
                current = next;
            }
        }

        heaps[i].clear();
    }
}

VkAllocation vkAlloc(VkMemoryRequirements requirements){

}

void vkFree(VkAllocation allocation){

}

VkAllocation vkHostAlloc(VkMemoryRequirements requirements){

}

void vkHostFree(VkAllocation allocation){

}
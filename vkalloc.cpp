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
    std::unordered_map<VkDeviceMemory, int32_t> pageMap;
    std::vector<Page> heaps[32];    //32 possible heaps
}

void vkaInit(VkPhysicalDevice physicalDevice, VkDevice device){

}

using namespace vka;

void vkaTerminate(){

}

VkAllocation vkAlloc(VkMemoryRequirements requirements){

}

void vkFree(VkAllocation allocation){

}

VkAllocation vkHostAlloc(VkMemoryRequirements requirements){

}

void vkHostFree(VkAllocation allocation){

}
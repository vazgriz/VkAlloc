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
    VkAllocationCallbacks* callbacks;
    std::unordered_map<VkDeviceMemory, int32_t> pageMap;    //caches index into one of the vector<Page> heaps
    std::vector<Page> heaps[32];    //32 possible heaps
}

void vkaInit(VkPhysicalDevice physicalDevice, VkDevice device){
    vka::physicalDevice = physicalDevice;
    vka::device = device;

    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &vka::properties);
}

using namespace vka;
static VkAllocation AttemptAlloc(std::vector<Page>& heap, VkMemoryRequirements requirements);
static VkAllocation AttemptAlloc(Page& page, VkMemoryRequirements requirements);

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
    for (size_t i = 0; i < properties.memoryTypeCount; i++) {
        if ((requirements.memoryTypeBits & (1 << i))
            && properties.memoryTypes[i].propertyFlags == VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) {
            std::vector<Page> &heap = heaps[i];

            VkAllocation result = AttemptAlloc(heap, requirements);
            if (result.deviceMemory != VK_NULL_HANDLE) {
                return result;
            }
        }
    }

    return {};
}

void vkFree(VkAllocation allocation){

}

VkAllocation vkHostAlloc(VkMemoryRequirements requirements){

}

void vkHostFree(VkAllocation allocation){

}

static VkAllocation AttemptAlloc(std::vector<Page>& heap, VkMemoryRequirements requirements) {

}

static VkAllocation AttempAlloc(Page& page, VkMemoryRequirements requirements) {

}
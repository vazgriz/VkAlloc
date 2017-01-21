#include "include/vkaAllocator.h"

using namespace vka;

Allocator::Allocator(VkPhysicalDevice physicalDevice, VkDevice device, VkAllocationCallbacks* callbacks, size_t pageSize) {
    this->physicalDevice = physicalDevice;
    this->device = device;
    this->callbacks = callbacks;

    VkPhysicalDeviceMemoryProperties props;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &props);

    for (size_t i = 0; i < props.memoryHeapCount; i++) {
        heaps.emplace_back(new Heap(static_cast<uint32_t>(i), pageSize, props));
    }
}

VkaAllocation Allocator::Alloc(VkMemoryRequirements requirements, VkMemoryPropertyFlags flags) {
    for (size_t i = 0; i < heaps.size(); i++) {
        uint32_t typeIndex;
        if (heaps[i]->Match(requirements, flags, &typeIndex)) {
            VkaAllocation result = heaps[i]->Alloc(requirements, typeIndex);
            if (result.memory != VK_NULL_HANDLE) {
                return result;
            }
        }
    }

    return {};
}

void Allocator::Free(VkaAllocation allocation) {

}
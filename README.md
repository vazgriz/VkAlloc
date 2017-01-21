# VkAlloc
Provides a simple API to allocate memory for Vulkan. It will suballocate regions of memory from a large VkDeviceMemory object.

## Usage
`VkaAllocation` is defined as
```
struct VkaAllocation {
   VkDeviceMemory memory;
   size_t offset;
   size_t size;
};
```
`offset` is the start of the allocated region within the `memory` object.
`size` is the size of allocated region in bytes.
This struct is used to handle all allocations from this library.

`vka::Allocator(VkPhysicalDevice physicalDevice, VkDevice device, size_t pageSize)` constructs the allocator.

`vka::Allocator.Alloc(VkMemoryRequirements requirements, VkMemoryPropertyFlags flag)` allocates a a region of memory that satisfies the given requirements and flags. This function returns a VkaAllocation object. If the allocation failed, the `memory` field of the returned value will be `VK_NULL_HANDLE`.

`vka::Allocator.Free(VkaAllocation allocation)` will free the given allocation.

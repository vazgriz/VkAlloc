# VkAlloc
Provides a simple API to allocate memory for Vulkan

## Usage
`vkaInit` initializes the library with the given PhysicalDevice and Device

`vkaTerminate` terminates the library and releases all memory on the CPU and GPU used by this library. Be sure that all buffers allocated with this library will not be used after calling vkaTerminate

`vkaAlloc` allocates memory on that sastifies the given `VkMemoryRequirements` and `VkMemoryPropertyFlags` arguments, and returns a VkAllocation object

`vkaFree` frees the memory pointed to by the given VkAllocation object

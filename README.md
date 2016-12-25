# VkAlloc
Provides a simple API to allocate memory for Vulkan

## Usage
`vkaInit` initializes the library with the given PhysicalDevice and Device

`vkaTerminate` terminates the library and releases all memory on the CPU and GPU used by this library. Be sure that all buffers allocated with this library will not be used after calling vkaTerminate

`vkaAlloc` allocates memory on the device (`VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT`) and returns a VkAllocation object

`vkaAllocHost` allocates memory that on the host (`VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT`) and returns a VkAllocation object

`vkaAllocFlags` allocates memory that satisfies the given flags and returns a VkAllocation object

`vkaFree` frees the memory pointed to by the given VkAllocation object

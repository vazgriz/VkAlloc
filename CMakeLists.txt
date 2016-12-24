cmake_minimum_required(VERSION 3.6)
project(VkAlloc)

set(CMAKE_CXX_STANDARD 11)
include_directories($ENV{VULKAN_SDK}/include)
add_library(VkAlloc ${SOURCE_FILES} include/vkalloc.h vkalloc.c)
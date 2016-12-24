#include "include/vkalloc.h"

typedef struct Page Page;
typedef struct Node Node;

struct Page{
    Page* next;
    Node* head;
    VkDeviceMemory memory;
};

struct Node{
    Node* next;
    uint64_t offset;
    uint64_t size;
};

VkPhysicalDevice vkaPhysicalDevice;
VkDevice vkaDevice;
VkPhysicalDeviceMemoryProperties memProperties;
Page* vkaPageHeads[32];   //there can be a max of 32 different memory heaps

const VkAllocation EmptyAllocation;

//can't be [0..31]
#define INDEX_NOT_FOUND 32

static void FreePage(Page* page);
static uint32_t FindIndex(uint32_t mask, VkMemoryPropertyFlags flags);
static VkAllocation AttemptAllocFromHeap(uint32_t index, uint64_t size);
static VkAllocation AttemptAllocFromPage(Page* page, uint64_t size);
static Page* AttemptAllocPage(Page** slot, uint32_t index, uint64_t size);
static VkAllocation Split(Node** head, uint64_t size);

void vkaInit(VkPhysicalDevice physicalDevice, VkDevice device){
    vkaPhysicalDevice = physicalDevice;
    vkaDevice = device;

    for (uint32_t i = 0; i < 32; i++){
        vkaPageHeads[i] = NULL;
    }

    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);
}

void vkaTerminate(){
    for (uint32_t i = 0; i < 32; i++){
        if (vkaPageHeads[i]){
            Page* head = vkaPageHeads[i];
            while (head) {
                Page* old = head;
                head = head->next;
                FreePage(old);
            }
        }
    }
}

VkAllocation vkAlloc(uint64_t size, uint32_t mask){
    VkAllocation result = EmptyAllocation;
    uint32_t index = FindIndex(mask, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    if (index == INDEX_NOT_FOUND){
        return result;
    } else {
        return AttemptAllocFromHeap(index, size);
    }
}

void vkFree(VkAllocation allocation){

}

VkAllocation vkHostAlloc(uint64_t size, uint32_t mask){
    VkAllocation result = EmptyAllocation;
    uint32_t index = FindIndex(mask, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);

    if (index == INDEX_NOT_FOUND){
        return result;
    } else {
        return AttemptAllocFromHeap(index, size);
    }
}

void vkHostFree(VkAllocation allocation){

}

static void FreePage(Page* page){
    Node* head = page->head;
    while (head) {
        Node* old = head;
        head = head->next;
        free(old);
    }
    free(page);
}

static uint32_t FindIndex(uint32_t mask, VkMemoryPropertyFlags flags){
    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((mask & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & flags) == flags) {
            return i;
        }
    }

    return INDEX_NOT_FOUND;
}

static VkAllocation AttemptAllocFromHeap(uint32_t index, uint64_t size){
    Page* head = vkaPageHeads[index];
    Page** last = &vkaPageHeads[index];
    while (head) {
        last = &(head->next);
        VkAllocation alloc = AttemptAllocFromPage(head, size);
        if (alloc.deviceMemory != VK_NULL_HANDLE){
            return alloc;
        }
        head = head->next;
    }

    Page* newPage = AttemptAllocPage(last, index, size);
    if (newPage) {
        return AttemptAllocFromPage(newPage, size);
    } else{
        return EmptyAllocation;
    }
}

static VkAllocation AttemptAllocFromPage(Page* page, uint64_t size){
    Node* current = page->head;
    Node** last = &(page->head);

    while (current) {
        if (current->size >= size){
            Split(last, size);
        }
        last = &(current->next);
        current = current->next;
    }

    return (VkAllocation){.deviceMemory = VK_NULL_HANDLE};
}

static VkAllocation Split(Node** head, uint64_t size){
    uint64_t amountLeft = (*head)->size - size;
    Node* current = *head;

    if (amountLeft == 0){
        free(current);
        head = &((*head)->next);
    } else {
        current->offset += size;
        current->size = amountLeft;
    }
}

static Page* AttemptAllocPage(Page** slot, uint32_t index, uint64_t size){
    uint64_t allocSize = VKA_ALLOC_SIZE;
    if (size > VKA_ALLOC_SIZE){
        allocSize = size;
    }

    VkMemoryAllocateInfo info;
    info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    info.pNext = NULL;
    info.allocationSize = allocSize;
    info.memoryTypeIndex = index;

    VkDeviceMemory deviceMemory;
    VkResult result = vkAllocateMemory(vkaDevice, &info, NULL, &deviceMemory);

    if (result == VK_SUCCESS){
        Node* newNode = malloc(sizeof(Node));
        *newNode = (Node){.size = allocSize};

        Page* newPage = malloc(sizeof(Page));
        *newPage = (Page){.memory = deviceMemory, .head = newNode};
    } else {
        return NULL;
    }
}
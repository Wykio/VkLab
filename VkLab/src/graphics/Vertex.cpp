#include "graphics/Vertex.h"

void VertexBuffer::initialize(Device* pdevice) {
    VkDevice logicalDevice = pdevice->getLogicalDevice();

    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = sizeof(vertices[0]) * vertices.size();
    bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT; // Vertex Buffer
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE; // The buffer will only be used from the graphics queue

    // Create the vertex buffer
    if (vkCreateBuffer(logicalDevice, &bufferInfo, nullptr, &vertexBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to created vertex buffer!");
    }
    
    // The first step of allocating memory for the buffer is to query its memory requirements
    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(logicalDevice, vertexBuffer, &memRequirements); // Get requirements for device and vertex buffer
    // memRequirements.size -> Size of the required amount of memory in bytes
    // alignment ->  Offset in bytes where the buffer begins in the allocated region of memory
    // memoryTypeBits -> Bit field of the memory types that are suitable for the buffer

    // Set allocation infos
    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(pdevice, memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    // VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT -> Map it so we can write to it from the CPU

    // Allocate memory
    if (vkAllocateMemory(logicalDevice, &allocInfo, nullptr, &vertexBufferMemory) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate vertex buffer memory!");
    }

    // Associate this memory with the buffer
    vkBindBufferMemory(logicalDevice, vertexBuffer, vertexBufferMemory, 0);

    // Fill the vertex buffer
    void* data;
    // copy the vertex data to the buffer.This is done by mapping the buffer memory into CPU accessible memory
    vkMapMemory(logicalDevice, vertexBufferMemory, 0, bufferInfo.size, 0, &data);
    // Memcpy the vertex data to the mapped memory and unmap it again
    memcpy(data, vertices.data(), (size_t)bufferInfo.size);
    vkUnmapMemory(logicalDevice, vertexBufferMemory);
}

// Memory that is bound to a buffer object may be freed once the buffer is no longer used
void VertexBuffer::cleanup(Device* pdevice) {
    VkDevice logicalDevice = pdevice->getLogicalDevice();

    vkDestroyBuffer(logicalDevice, vertexBuffer, nullptr);
    vkFreeMemory(logicalDevice, vertexBufferMemory, nullptr);
}

VkBuffer VertexBuffer::getVertexBuffer() {
    return vertexBuffer;
}

// TypeFilter parameter is used to specify the bit field of memory types that are suitable
uint32_t VertexBuffer::findMemoryType(Device* pdevice, uint32_t typeFilter, VkMemoryPropertyFlags properties) {
    // Query info about the available types of memory
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(pdevice->getPhysicalDevice(), &memProperties);

    // Find a memory type that is suitable for the buffer itself
    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        // Checking if the corresponding bit is set to 1 AND also check for the support of properties in propertyFlags of that memory type
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }
    
    throw std::runtime_error("failed to find suitable memory type!");
}
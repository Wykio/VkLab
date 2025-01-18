#include "graphics/VertexBuffer.h"

void VertexBuffer::initialize(Device* pdevice) {
    VkDevice logicalDevice = pdevice->getLogicalDevice();

    VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

    //VkBuffer stagingBuffer;
    //VkDeviceMemory stagingBufferMemory;
    createBuffer(pdevice,
        bufferSize,
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        vertexBuffer,
        vertexBufferMemory);

    // Fill the vertex buffer
    void* data;
    // copy the vertex data to the buffer.This is done by mapping the buffer memory into CPU accessible memory
    vkMapMemory(logicalDevice, vertexBufferMemory, 0, bufferSize, 0, &data);
    // Memcpy the vertex data to the mapped memory and unmap it again
    memcpy(data, vertices.data(), (size_t) bufferSize);
    vkUnmapMemory(logicalDevice, vertexBufferMemory);

    //createBuffer(pdevice,
    //    )
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

//// TypeFilter parameter is used to specify the bit field of memory types that are suitable
//uint32_t VertexBuffer::findMemoryType(Device* pdevice, uint32_t typeFilter, VkMemoryPropertyFlags properties) {
//    // Query info about the available types of memory
//    VkPhysicalDeviceMemoryProperties memProperties;
//    vkGetPhysicalDeviceMemoryProperties(pdevice->getPhysicalDevice(), &memProperties);
//
//    // Find a memory type that is suitable for the buffer itself
//    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
//        // Checking if the corresponding bit is set to 1 AND also check for the support of properties in propertyFlags of that memory type
//        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
//            return i;
//        }
//    }
//    
//    throw std::runtime_error("failed to find suitable memory type!");
//}
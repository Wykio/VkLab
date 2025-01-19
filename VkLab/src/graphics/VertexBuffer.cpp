#include "graphics/VertexBuffer.h"

void VertexBuffer::initialize(Device* pdevice, CommandPools* pcommandPools) {
    VkDevice logicalDevice = pdevice->getLogicalDevice();

    VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

    // Create staging buffer
    VkBuffer stagingBuffer; // For mapping and copying the vertex data.
    VkDeviceMemory stagingBufferMemory; 
    createBuffer(
        pdevice,
        bufferSize,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT, // Buffer can be used as source in a memory transfer operation
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        stagingBuffer,
        stagingBufferMemory
    );

    // Copy the vertex data to the buffer.
    void* data;
    // This is done by mapping the buffer memory into CPU accessible memory
    vkMapMemory(logicalDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
    // Memcpy the vertex data to the mapped memory and unmap it again
    memcpy(data, vertices.data(), (size_t) bufferSize);
    vkUnmapMemory(logicalDevice, stagingBufferMemory);

    createBuffer(
        pdevice,
        bufferSize,
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, // Buffer can be used as destination in a memory transfer operation
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, // The vertexBuffer is now allocated from a memory type that is device local
        vertexBuffer,
        vertexBufferMemory
    );

    copyBuffer(pdevice, pcommandPools->getTransferCommandPool(), stagingBuffer, vertexBuffer, bufferSize);

    vkDestroyBuffer(logicalDevice, stagingBuffer, nullptr);
    vkFreeMemory(logicalDevice, stagingBufferMemory, nullptr);
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


#include "graphics/BufferManager.h"

void BufferManager::initialize(Device* pdevice, CommandPools* pcommandPools) {
    createVertexBuffer(pdevice, pcommandPools);
    createIndexBuffer(pdevice, pcommandPools);
    createUniformBuffer(pdevice);
}

// Memory that is bound to a buffer object may be freed once the buffer is no longer used
void BufferManager::cleanup(Device* pdevice) {
    VkDevice logicalDevice = pdevice->getLogicalDevice();

    vkDestroyBuffer(logicalDevice, indexBuffer, nullptr);
    vkFreeMemory(logicalDevice, indexBufferMemory, nullptr);

    vkDestroyBuffer(logicalDevice, vertexBuffer, nullptr);
    vkFreeMemory(logicalDevice, vertexBufferMemory, nullptr);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroyBuffer(logicalDevice, uniformBuffers[i], nullptr);
        vkFreeMemory(logicalDevice, uniformBuffersMemory[i], nullptr);
    }
}

void BufferManager::updateUniformBuffer(SwapChain swapchain, uint32_t currentImage) {
    //  Calculate the time in seconds since rendering has started with floating point accuracy
    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

    // Define the model, view and projection transformations
    UniformBufferObject ubo{};
    ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)); // Accomplishes the purpose of rotation 90 degrees per second
    ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)); // Look at the geometry from above at a 45 degree angle
    // Configure FOV, aspect ratio, near view plane, far view plane ..
    // Use the current swap chain extent to calculate the aspect ratio to take into account the new width and height of the window after a resize
    ubo.proj = glm::perspective(glm::radians(45.0f), swapchain.getSwapChainExtent().width / (float) swapchain.getSwapChainExtent().height, 0.1f, 10.0f);

    // GLM was originally designed for OpenGL, where the Y coordinate of the clip coordinates is inverted
    // The easiest way to compensate for that is to flip the sign on the scaling factor of the Y axis in the projection matrix
    // If you don’t do this, then the image will be rendered upside down
    ubo.proj[1][1] *= -1;

    // We only map the uniform buffer once, so we can directly write to it without having to map again
    memcpy(uniformBuffersMapped[currentImage], &ubo, sizeof(ubo));

    // Using a UBO this way is not the most efficient way to pass frequently changing values to the shader
    // A more efficient way to pass a small buffer of data to shaders are push constants
}

VkBuffer BufferManager::getVertexBuffer() {
    return vertexBuffer;
}

VkBuffer BufferManager::getIndexBuffer() {
    return indexBuffer;
}

std::vector<VkBuffer> BufferManager::getUniformBuffer() {
    return uniformBuffers;
}

void BufferManager::createVertexBuffer(Device* pdevice, CommandPools* pcommandPools) {
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
    memcpy(data, vertices.data(), (size_t)bufferSize);
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

void BufferManager::createIndexBuffer(Device* pdevice, CommandPools* pcommandPools) {
    VkDevice logicalDevice = pdevice->getLogicalDevice();

    VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

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
    memcpy(data, indices.data(), (size_t)bufferSize);
    vkUnmapMemory(logicalDevice, stagingBufferMemory);

    createBuffer(
        pdevice,
        bufferSize,
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, // Buffer can be used as destination in a memory transfer operation
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, // The vertexBuffer is now allocated from a memory type that is device local
        indexBuffer,
        indexBufferMemory
    );

    copyBuffer(pdevice, pcommandPools->getTransferCommandPool(), stagingBuffer, indexBuffer, bufferSize);

    vkDestroyBuffer(logicalDevice, stagingBuffer, nullptr);
    vkFreeMemory(logicalDevice, stagingBufferMemory, nullptr);
}

void BufferManager::createUniformBuffer(Device* pdevice) {
    VkDevice logicalDevice = pdevice->getLogicalDevice();

    VkDeviceSize buffersize = sizeof(UniformBufferObject);

    // We need to have as many uniform buffers as we have frames in flight
    uniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
    uniformBuffersMemory.resize(MAX_FRAMES_IN_FLIGHT);
    uniformBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        createBuffer(
            pdevice,
            buffersize,
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            uniformBuffers[i],
            uniformBuffersMemory[i]
        );

        // We map the buffer right after creation using vkMapMemory to get a pointer to which we can write the data later on
        // The buffer stays mapped to this pointer for the application’s whole lifetime
        // This technique is called "persistent mapping" and works on all Vulkan implementations
        // Not having to map the buffer every time we need to update it increases performances, as mapping is not free
        vkMapMemory(logicalDevice, uniformBuffersMemory[i], 0, buffersize, 0, &uniformBuffersMapped[i]);
    }
}
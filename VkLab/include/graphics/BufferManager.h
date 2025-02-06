#ifndef VERTEX_H
#define VERTEX_H

#include "core/Device.h"
#include "core/Constant.h"
#include "graphics/SwapChain.h"
#include "graphics/DescriptorSet.h"
#include "utils/Buffer.h"

#include <vulkan/vulkan.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <array>
#include <chrono>

struct Vertex
{
	glm::vec2 pos;
	glm::vec3 color;

    // Describes at which rate to load data from memory throughout the vertices
    // It specifies the number of bytes between data entries and whether to move to the next data entry after each vertex or after each instance
    static VkVertexInputBindingDescription getBindingDescription() { // static because it's global the Vertex structure
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex); // Number of bytes from one entry to the next
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX; // Move to the next data entry after each vertex

        return bindingDescription;
    }

    // Describes how to extract a vertex attribute from a chunk of vertex data originating from a binding description
    static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions() {
        std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};
        // Positions
        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0; // References the location directive of the input in the vertex shader
        attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT; // vec2: VK_FORMAT_R32G32_SFLOAT
        attributeDescriptions[0].offset = offsetof(Vertex, pos);
        // Colors
        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex, color);

        return attributeDescriptions;
    }
};

const std::vector<Vertex> vertices = {
    {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
    {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
    {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
    {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
};

const std::vector<uint16_t> indices = {
    0, 1, 2, 2, 3, 0
};

class BufferManager
{
public:
    void initialize(Device* pdevice, CommandPools* pcommandPools);
    void cleanup(Device* pdevice);
    void updateUniformBuffer(SwapChain swapchain, uint32_t currentImage);
    VkBuffer getVertexBuffer();
    VkBuffer getIndexBuffer();
    std::vector<VkBuffer> getUniformBuffer();

private: // Note: Try to create a single buffer for both of these with offsets for memory optimisation
    void createVertexBuffer(Device* pdevice, CommandPools* pcommandPools);
    void createIndexBuffer(Device* pdevice, CommandPools* pcommandPools);
    void createUniformBuffer(Device* pdevice);

    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;

    VkBuffer indexBuffer;
    VkDeviceMemory indexBufferMemory;

    std::vector<VkBuffer> uniformBuffers;
    std::vector<VkDeviceMemory> uniformBuffersMemory;
    std::vector<void*> uniformBuffersMapped;
};

#endif // VERTEX_H
#ifndef VERTEX_H
#define VERTEX_H

#include "core/Device.h"
#include "utils/Buffer.h"

#include <vulkan/vulkan.h>
#include <glm/glm.hpp>
#include <array>

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

const std::vector<Vertex> vertices = { // interleaving vertex attributes
    {{0.0f, -0.5f}, {1.0f, 1.0f, 1.0f}},
    {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
    {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
};

class VertexBuffer
{
public:
    void initialize(Device* pdevice);
    void cleanup(Device* pdevice);
    VkBuffer getVertexBuffer();

private:
    //uint32_t findMemoryType(Device* pdevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);

    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;
};

#endif // VERTEX_H
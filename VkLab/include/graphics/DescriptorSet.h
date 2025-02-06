#ifndef DESCRIPTOR_SET_H
#define DESCRIPTOR_SET_H

#include "core/Constant.h"
#include "core/Device.h"
#include "graphics/DescriptorPool.h"
#include "graphics/BufferManager.h"

#include <vulkan/vulkan.h>
#include <glm/glm.hpp>

class BufferManager;

struct UniformBufferObject {
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
};

// A Descriptor Set is a collection of descriptors that tell shaders where and how to access resources(buffers, images, samplers, etc.)
// It serves as a link between a shader and its associated resources
class DescriptorSet
{
public:
	void initialize(Device* pdevice);
    void cleanup(Device* pdevice);
    void allocate(Device* pdevice, DescriptorPool* descriptorPool, BufferManager* bufferManager);
    VkDescriptorSetLayout* getDescriptorSetLayoutPtr();
    VkDescriptorSet* getDescriptorSetPtr(uint32_t index);

private:
    VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;

    std::vector<VkDescriptorSet> descriptorSets;
};

#endif // DESCRIPTOR_SET_H
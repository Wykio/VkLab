#include "graphics/DescriptorSet.h"

void DescriptorSet::initialize(Device* pdevice) {
    auto logicalDevice = pdevice->getLogicalDevice();

    VkDescriptorSetLayoutBinding uboLayoutBinding{};
    uboLayoutBinding.binding = 0;  // Specify the binding used in the shader
    uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uboLayoutBinding.descriptorCount = 1; // This could be used to specify a transformation for each of the bones in a skeleton for skeletal animation, for example.
    uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT; // Used in vertex shader
    uboLayoutBinding.pImmutableSamplers = nullptr; // Optional - Only relevent for image sampling

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = 1;
    layoutInfo.pBindings = &uboLayoutBinding;

    if (vkCreateDescriptorSetLayout(logicalDevice, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor set layout!");
    }
}

void DescriptorSet::cleanup(Device* pdevice) {
    vkDestroyDescriptorSetLayout(pdevice->getLogicalDevice(), descriptorSetLayout, nullptr);
}

// Allocate all descriptor Sets
void DescriptorSet::allocate(Device* pdevice, DescriptorPool* descriptorPool, BufferManager* bufferManager) {
    auto logicalDevice = pdevice->getLogicalDevice();

    std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, descriptorSetLayout);

    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptorPool->getDescriptorPool();
    allocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT); // One descriptor set for each frame in flight
    allocInfo.pSetLayouts = layouts.data();

    descriptorSets.resize(MAX_FRAMES_IN_FLIGHT);

    if (vkAllocateDescriptorSets(logicalDevice, &allocInfo, descriptorSets.data()) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate descriptor sets!");
    }

    // Configure and populate every descriptor
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        // Descriptors are configured with a VkDescriptorBufferInfo
        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = bufferManager->getUniformBuffer()[i];
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(UniformBufferObject);

        VkWriteDescriptorSet descriptorWrite{};
        descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite.dstSet = descriptorSets[i];
        descriptorWrite.dstBinding = 0;
        descriptorWrite.dstArrayElement = 0; // Remember that descriptors can be arrays, so we also need to specify the index
        descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrite.descriptorCount = 1;
        // The last field references an array with descriptorCount structs that actually configure the descriptors.
        // It depends on the type of descriptor which one of the three you actually need to use.
        descriptorWrite.pBufferInfo = &bufferInfo;
        descriptorWrite.pImageInfo = nullptr; // Optional
        descriptorWrite.pTexelBufferView = nullptr; // Optional

        vkUpdateDescriptorSets(logicalDevice, 1, &descriptorWrite, 0, nullptr);
    }
}

VkDescriptorSetLayout* DescriptorSet::getDescriptorSetLayoutPtr() {
    return &descriptorSetLayout;
}

VkDescriptorSet* DescriptorSet::getDescriptorSetPtr(uint32_t index) {
    return &descriptorSets[index];
}
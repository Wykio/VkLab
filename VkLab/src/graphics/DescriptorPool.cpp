#include "graphics/DescriptorPool.h"

void DescriptorPool::initialize() {
	// Describe which descriptor types our descriptor sets are going to contain
	VkDescriptorPoolSize poolSize{};
	poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	// We will allocate one of these descriptors for every frame
	poolSize.descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = 1; // Only one type of Descriptor (UBO)
	poolInfo.pPoolSizes = &poolSize;

	// Aside from the maximum number of individual descriptors that are available, 
	// we also need to specify the maximum number of descriptor sets that may be allocated
	poolInfo.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

	if (vkCreateDescriptorPool(RendererContext::getInstance().pdevice->getLogicalDevice(), &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor pool!");
	}
}

void DescriptorPool::cleanup() {
	vkDestroyDescriptorPool(RendererContext::getInstance().pdevice->getLogicalDevice(), descriptorPool, nullptr);
}

VkDescriptorPool DescriptorPool::getDescriptorPool() {
	return descriptorPool;
}
#include "graphics/CommandPool.h"

void CommandPool::initialize(Device* pdevice, VkSurfaceKHR* psurface) {
	QueueFamilyIndices queueFamilyIndices = findQueueFamilies(pdevice->getPhysicalDevice(), psurface);

	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT; // Allow command buffers to be rerecorded individually
	poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value(); //  We’re going to record commands for drawing (graphics queue family)

	if (vkCreateCommandPool(pdevice->getLogicalDevice(), &poolInfo, nullptr, &commandPool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create command pool!");
	}
}

void CommandPool::cleanup(Device* pdevice) {
	vkDestroyCommandPool(pdevice->getLogicalDevice(), commandPool, nullptr);
}
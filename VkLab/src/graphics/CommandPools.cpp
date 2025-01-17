#include "graphics/CommandPools.h"

void CommandPools::initialize(Device* pdevice, VkSurfaceKHR* psurface) {
	VkDevice logicalDevice = pdevice->getLogicalDevice();
	QueueFamilyIndices queueFamilyIndices = findQueueFamilies(pdevice->getPhysicalDevice(), psurface);

	// Drawing command pool create info
	VkCommandPoolCreateInfo drawPoolInfo{};
	drawPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	drawPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT; // Allow command buffers to be rerecorded individually
	drawPoolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value(); //  We’re going to record commands for drawing (graphics queue family)

	// Transfer command pool create info
	VkCommandPoolCreateInfo transferPoolInfo{};
	transferPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	transferPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	transferPoolInfo.queueFamilyIndex = queueFamilyIndices.transferFamily.value();

	// Create drawing command pool
	if (vkCreateCommandPool(logicalDevice, &drawPoolInfo, nullptr, &drawCommandPool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create draw command pool!");
	}

	// Create transfer command pool
	if (vkCreateCommandPool(logicalDevice, &transferPoolInfo, nullptr, &transferCommandPool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create transfer command pool!");
	}
}

void CommandPools::cleanup(Device* pdevice) {
	VkDevice logicalDevice = pdevice->getLogicalDevice();

	vkDestroyCommandPool(logicalDevice, drawCommandPool, nullptr);
	vkDestroyCommandPool(logicalDevice, transferCommandPool, nullptr);
}

VkCommandPool CommandPools::getDrawCommandPool() {
	return drawCommandPool;
}

VkCommandPool CommandPools::getTransferCommandPool() {
	return transferCommandPool;
}
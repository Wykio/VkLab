#ifndef BUFFER_H
#define BUFFER_H

#include "core/Device.h"
#include "graphics/CommandPools.h"

#include <vulkan/vulkan.h>

inline void createBuffer(Device* pdevice, VkDeviceSize deviceSize, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
inline uint32_t findMemoryType(Device* pdevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);

// Function to create many different types of buffers. The last two parameters are output variables to write the handles to.
inline void createBuffer(Device* pdevice, VkDeviceSize deviceSize, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory) {
	auto logicalDevice = pdevice->getLogicalDevice();
	
	QueueFamilyIndices indices = findQueueFamilies(RendererContext::getInstance().pdevice->getPhysicalDevice());
	uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.transferFamily.value() };

	// Create the buffer
	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = deviceSize;
	bufferInfo.usage = usage;

	if (indices.graphicsFamily != indices.transferFamily) {
		bufferInfo.sharingMode = VK_SHARING_MODE_CONCURRENT;
		bufferInfo.queueFamilyIndexCount = 2;
		bufferInfo.pQueueFamilyIndices = queueFamilyIndices;
		std::cout << "Transfer Queue Index is different than Graphic Queue Index" << std::endl;
	}
	else {
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	}

	if (vkCreateBuffer(logicalDevice, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
		throw std::runtime_error("failed to create buffer!");
	}

	// Query its memory requirements
	VkMemoryRequirements memoryRequirements;
	vkGetBufferMemoryRequirements(logicalDevice, buffer, &memoryRequirements);
	// memRequirements.size -> Size of the required amount of memory in bytes
	// alignment ->  Offset in bytes where the buffer begins in the allocated region of memory
	// memoryTypeBits -> Bit field of the memory types that are suitable for the buffer

	// Allocate the buffer
	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memoryRequirements.size;
	allocInfo.memoryTypeIndex = findMemoryType(pdevice, memoryRequirements.memoryTypeBits, properties);

	if (vkAllocateMemory(logicalDevice, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate buffer memory");
	}

	vkBindBufferMemory(logicalDevice, buffer, bufferMemory, 0);
}

// Copy the contents from one buffer to another using a transfer command pool
inline void copyBuffer(Device* pdevice, VkCommandPool commandPool, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {
	auto logicalDevice = pdevice->getLogicalDevice();
	auto transferQueue = pdevice->getTransferQueue();

	// Allocate a temporary command buffer
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = commandPool;
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer;
	vkAllocateCommandBuffers(logicalDevice, &allocInfo, &commandBuffer);

	// Start recording the commandBuffer;
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	// We’re only going to use the command buffer once and wait with returning from the function until the copy operation has finished executing
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(commandBuffer, &beginInfo);

	// Transfer buffer content
	VkBufferCopy copyRegion{};
	copyRegion.srcOffset = 0; // Optional
	copyRegion.dstOffset = 0; // Optional
	copyRegion.size = size;
	vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

	// End recording
	vkEndCommandBuffer(commandBuffer);

	// Send command buffer to the queue and wait for it to finish
	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	vkQueueSubmit(transferQueue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(transferQueue);

	// Free the commandBuffer
	vkFreeCommandBuffers(logicalDevice, commandPool, 1, &commandBuffer);
}

// typeFilter parameter is used to specify the bit field of memory types that are suitable
inline uint32_t findMemoryType(Device* pdevice, uint32_t typeFilter, VkMemoryPropertyFlags properties) {
	// Query about the available types of memory
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(pdevice->getPhysicalDevice(), &memProperties);

	// Find a memory type that is suitable according to VkMemoryPropertyFlags
	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
		// Checking if the corresponding bit is set to 1 AND also check for the support of properties in propertyFlags of that memory type
		if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
			return i;
		}
	}

	throw std::runtime_error("failed to find suitable memory type!");
}

#endif // BUFFER_H
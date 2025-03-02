#ifndef BUFFER_H
#define BUFFER_H

#include "core/Device.h"
#include "graphics/CommandPools.h"
#include "utils/CommandBuffersUtils.h"

#include <vulkan/vulkan.h>

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

// Copy the contents from one buffer to another using a command buffer
inline void copyBuffer(Device* pdevice, VkCommandPool commandPool, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {
	auto logicalDevice = pdevice->getLogicalDevice();
	auto transferQueue = pdevice->getTransferQueue();

	// Begin recording
	VkCommandBuffer commandBuffer = beginSingleTimeCommands(logicalDevice, commandPool);

	// Transfer buffer content
	VkBufferCopy copyRegion{};
	copyRegion.size = size;
	vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

	// End recording and submit to the transfer queue
	endSingleTimeCommands(logicalDevice, transferQueue, commandPool, commandBuffer);
}

// Copy the contents from a buffer to an image
inline void copyBufferToImage(Device* pdevice, VkCommandPool commandPool, VkBuffer buffer, VkImage image, uint32_t width, uint32_t height) {
	auto logicalDevice = pdevice->getLogicalDevice();
	auto transferQueue = pdevice->getTransferQueue();

	VkCommandBuffer commandBuffer = beginSingleTimeCommands(logicalDevice, commandPool);

	// Specify which part of the buffer is going to be copied to which part of the image
	VkBufferImageCopy region{};
	region.bufferOffset = 0;
	// Specify how the pixels are laid out in memory
	region.bufferRowLength = 0;
	region.bufferImageHeight = 0; 

	// Indicate to which part of the image we want to copy the pixels
	region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.imageSubresource.mipLevel = 0;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount = 1;

	region.imageOffset = { 0, 0, 0 };
	region.imageExtent = {
		width,
		height,
		1
	};

	// Buffer to image copy operations are enqueued using the vkCmdCopyBufferToImage function
	vkCmdCopyBufferToImage(
		commandBuffer,
		buffer,
		image,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		1,
		&region
	);

	endSingleTimeCommands(logicalDevice, transferQueue, commandPool, commandBuffer);
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
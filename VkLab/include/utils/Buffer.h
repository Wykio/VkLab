#include "core/Device.h"

#include <vulkan/vulkan.h>

static void createBuffer(Device* pdevice, VkDeviceSize deviceSize, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
static uint32_t findMemoryType(Device* pdevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);

// Function to create many different types of buffers. The last two parameters are output variables to write the handles to.
static void createBuffer(Device* pdevice, VkDeviceSize deviceSize, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory) {
	auto logicalDevice = pdevice->getLogicalDevice();

	// Create the buffer
	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = deviceSize;
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

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

// typeFilter parameter is used to specify the bit field of memory types that are suitable
static uint32_t findMemoryType(Device* pdevice, uint32_t typeFilter, VkMemoryPropertyFlags properties) {
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
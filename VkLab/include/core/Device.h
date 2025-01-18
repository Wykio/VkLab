#ifndef DEVICE_H
#define DEVICE_H

#include "core/RendererContext.h"
#include "core/VulkanInstance.h"

#include <vulkan/vulkan.h>
#include <iostream>
#include <vector>
#include <optional>
#include <set>

// List all needed device extensions
const std::vector<const char*> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

// Struct used to store QueueFamily indices
struct QueueFamilyIndices {
	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily;
	std::optional<uint32_t> transferFamily;

	bool isComplete() {
		return graphicsFamily.has_value() && presentFamily.has_value() && transferFamily.has_value();
	}
};

// Struct used to query SwapChain support
struct SwapChainSupportDetails {
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};

class Device {
public:
	Device();
	~Device();
	void initialize(VkInstance instance);
	void cleanup();
	void pickPhysicalDevice(VkInstance instance);
	void createLogicalDevice();
	VkPhysicalDevice getPhysicalDevice();
	VkDevice getLogicalDevice();

private:
	VkSurfaceKHR* psurface;

	VkPhysicalDevice physicalDevice;

	VkDevice device;

	QueueFamilyIndices queueFamilyIndices;

	VkQueue graphicsQueue = VK_NULL_HANDLE;
	VkQueue presentQueue = VK_NULL_HANDLE;
	VkQueue transferQueue = VK_NULL_HANDLE;
};

// Device selection functions
bool isDeviceSuitable(VkPhysicalDevice physicalDevice);
QueueFamilyIndices findQueueFamilies(VkPhysicalDevice physicalDevice);
bool checkDeviceExtensionSupport(VkPhysicalDevice physicalDevice);
SwapChainSupportDetails querySwapChainSupport(const VkPhysicalDevice device, const VkSurfaceKHR* psurface);

#endif // DEVICE_H
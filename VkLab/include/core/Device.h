#ifndef DEVICE_H
#define DEVICE_H

#include "core/VulkanInstance.h"

#include <vulkan/vulkan.h>
#include <iostream>
#include <vector>
#include <optional>
#include <set>

const std::vector<const char*> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

struct QueueFamilyIndices {
	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily;

	bool isComplete() {
		return graphicsFamily.has_value() && presentFamily.has_value();
	}
};

struct SwapChainSupportDetails {
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};

class Device {
public:
	Device();
	~Device();
	void initialize(VkInstance instance, VkSurfaceKHR* psurface, VkQueue* pgraphicsQueue, VkQueue* ppresentQueue);
	void cleanup();
	void pickPhysicalDevice(VkInstance instance);
	void createLogicalDevice(VkQueue* graphicsQueue, VkQueue* presentQueue);
	VkPhysicalDevice getPhysicalDevice();
	VkDevice getLogicalDevice();

private:
	VkPhysicalDevice physicalDevice;
	VkDevice device;
	VkSurfaceKHR *psurface;
};

// Device selection functions
bool isDeviceSuitable(VkPhysicalDevice physicalDevice, VkSurfaceKHR* psurface);
QueueFamilyIndices findQueueFamilies(VkPhysicalDevice physicalDevice, VkSurfaceKHR* psurface);
bool checkDeviceExtensionSupport(VkPhysicalDevice physicalDevice);
SwapChainSupportDetails querySwapChainSupport(const VkPhysicalDevice device, const VkSurfaceKHR* psurface);

#endif // DEVICE_H
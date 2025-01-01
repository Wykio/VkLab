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

class Device {
public:
	Device();
	~Device();
	void initialize(VkInstance instance, VkSurfaceKHR* psurface, VkQueue* pgraphicsQueue, VkQueue* ppresentQueue);
	void cleanup();
	void pickPhysicalDevice(VkInstance instance);
	void createLogicalDevice(VkQueue* graphicsQueue, VkQueue* presentQueue);

private:
	VkPhysicalDevice physicalDevice;
	VkDevice device;
	VkSurfaceKHR *psurface;
};

bool isDeviceSuitable(VkPhysicalDevice physicalDevice, VkSurfaceKHR* psurface);
QueueFamilyIndices findQueueFamilies(VkPhysicalDevice physicalDevice, VkSurfaceKHR* psurface);
bool checkDeviceExtensionSupport(VkPhysicalDevice physicalDevice);

#endif // DEVICE_H
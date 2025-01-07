#ifndef SWAPCHAIN_H
#define SWAPCHAIN_H

#include "Device.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>
#include <algorithm> // Necessary for std::clamp


class SwapChain
{
public:
	void initialize(Device* device, VkSurfaceKHR* psurface, GLFWwindow* window);
	void cleanup(Device* device);
	const std::vector<VkImage> getSwapChainImages();
	const VkFormat getSwapChainImageFormat();
	
private:
	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, GLFWwindow* window);

	VkSwapchainKHR swapChain;
	std::vector<VkImage> swapChainImages;
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;
};

#endif // SWAPCHAIN_H
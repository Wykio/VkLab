#ifndef SWAPCHAIN_H
#define SWAPCHAIN_H

#include "core/Device.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>
#include <algorithm> // Necessary for std::clamp

class ImageViews; //

class SwapChain
{
public:
	void initialize(GLFWwindow* window, Device* pdevice);
	void cleanup(Device* device);
	const VkSwapchainKHR getSwapChain();
	const std::vector<VkImage> getSwapChainImages();
	const VkFormat getSwapChainImageFormat();
	const VkExtent2D getSwapChainExtent();
	
private:
	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
	VkExtent2D chooseSwapExtent(GLFWwindow* pwindow, const VkSurfaceCapabilitiesKHR& capabilities);

	VkSwapchainKHR swapChain = VK_NULL_HANDLE;
	std::vector<VkImage> swapChainImages;
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;
};

#endif // SWAPCHAIN_H
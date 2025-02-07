#ifndef SWAPCHAIN_H
#define SWAPCHAIN_H

#include "core/Device.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>
#include <algorithm> // Necessary for std::clamp

class ImageViews;

// The SwapChain is the infrastructure that will own the buffers we will render to before we visualize them on the screen
// It is essentially a queue of images that are waiting to be presented to the screen. Our application will acquire
// such an image to draw to it, and then return it to the queue
class SwapChain
{
public:
	void initialize(GLFWwindow* window);
	void cleanup();
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
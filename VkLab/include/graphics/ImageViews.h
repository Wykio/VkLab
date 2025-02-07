#ifndef IMAGE_VIEWS_H
#define IMAGE_VIEWS_H

#include "graphics/Swapchain.h"

#include <vulkan/vulkan.h>
#include <vector>
#include <algorithm> // Necessary for std::clamp

// To use any VkImage, including those in the swap chain, in the render pipeline we have to create a VkImageView object
// An image view is quite literally a view into an image.
// It describes how to access the image and which part of the image to access,
// for example if it should be treated as a 2D texture depth texture without any mipmapping levels.

class ImageViews
{
public:
	void initialize(SwapChain* swapchain);
	void cleanup();
	std::vector<VkImageView> getSwapChainImageViews();

private:
	std::vector<VkImageView> swapChainImageViews;
};

#endif
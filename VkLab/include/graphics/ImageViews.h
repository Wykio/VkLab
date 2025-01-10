#ifndef IMAGE_VIEWS_H
#define IMAGE_VIEWS_H

#include "graphics/Swapchain.h"

#include <vulkan/vulkan.h>
#include <vector>
#include <algorithm> // Necessary for std::clamp


class ImageViews
{
public:
	void initialize(Device* device, SwapChain* swapchain);
	void cleanup(Device* device);
	std::vector<VkImageView> getSwapChainImageViews();

private:
	std::vector<VkImageView> swapChainImageViews;
};

#endif
#ifndef IMAGE_VIEWS_H
#define IMAGE_VIEWS_H

#include "core/Swapchain.h"

#include <vulkan/vulkan.h>
#include <vector>
#include <algorithm> // Necessary for std::clamp


class ImageViews
{
public:
	void initialize(Device* device, SwapChain* swapchain);
	void cleanup(Device* device);

private:
	std::vector<VkImageView> swapChainImageViews;
};

#endif
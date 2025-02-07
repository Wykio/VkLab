#include "graphics/ImageViews.h"

// Create an image view for each swapchainImages
void ImageViews::initialize(SwapChain* swapchain) {
	std::vector<VkImage> swapchainImages = swapchain->getSwapChainImages();
	swapChainImageViews.resize(swapchainImages.size());

	for (size_t i = 0; i < swapchainImages.size(); i++) {
		// Create an image view
		VkImageViewCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = swapchain->getSwapChainImages()[i];
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D; // 2D textures
		createInfo.format = swapchain->getSwapChainImageFormat();

		// Allows you to swizzle the color channels around. For example,
		// you can map all of the channels to the red channel for a monochrome texture.
		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

		// The subresourceRange field describes what the image’s purpose is and which part of the image should be accessed.
		// Our images will be used as color targets without any mipmapping levels or multiple layers.
		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;

		if (vkCreateImageView(RendererContext::getInstance().pdevice->getLogicalDevice(), &createInfo, nullptr, &swapChainImageViews[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to create image views!");
		}
	}
}

// Destroy every image views
void ImageViews::cleanup() {
	for (auto imageView : swapChainImageViews) {
		vkDestroyImageView(RendererContext::getInstance().pdevice->getLogicalDevice(), imageView, nullptr);
	}
}

std::vector<VkImageView> ImageViews::getSwapChainImageViews() {
	return swapChainImageViews;
}
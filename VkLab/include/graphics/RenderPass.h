#ifndef RENDERPASS_H
#define RENDERPASS_H

#include "graphics/SwapChain.h"

#include <vulkan/vulkan.h>

class RenderPass
{
public:
	void initialize(Device* pdevice, SwapChain* pswapchain);
	void cleanup(Device* pdevice);
	VkRenderPass getRenderPass();

private:
	VkRenderPass renderPass;
};

#endif // RENDERPASS_H
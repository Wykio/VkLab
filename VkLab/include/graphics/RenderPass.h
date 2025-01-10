#ifndef RENDERPASS_H
#define RENDERPASS_H

#include "core/SwapChain.h"

#include <vulkan/vulkan.h>

class RenderPass
{
public:
	void initialize(Device* pdevice, SwapChain* pswapchain);
	void cleanup(Device* pdevice);

private:
	VkRenderPass renderPass;
};

#endif // RENDERPASS_H
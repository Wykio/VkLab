#ifndef RENDERPASS_H
#define RENDERPASS_H

#include "graphics/SwapChain.h"

#include <vulkan/vulkan.h>

// Before we can finish creating the pipeline, we need to tell Vulkan about the framebuffer attachments that will be used while rendering.
// We need to specify how many color and depth buffers there will be,
// how many samples to use for each of them and how their contents should be handled throughout the rendering operations.
// All of this information is wrapped in a render pass object
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
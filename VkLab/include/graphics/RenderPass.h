#ifndef RENDERPASS_H
#define RENDERPASS_H

#include "graphics/SwapChain.h"

#include <vulkan/vulkan.h>

// In Vulkan, a render pass is a fundamental construct that defines a sequence of operations on a set of attachments,
// such as color and depth buffers, over one or more subpasses
// 
// It provides the GPU with detailed information about the rendering process, enabling optimizations and efficient resource management
// 
// Before we can finish creating the pipeline, we need to tell Vulkan about the framebuffer attachments that will be used while rendering.
// We need to specify how many color and depth buffers there will be,
// how many samples to use for each of them and how their contents should be handled throughout the rendering operations.
// All of this information is wrapped in a render pass object
class RenderPass
{
public:
	void initialize(SwapChain* pswapchain);
	void cleanup();
	VkRenderPass getRenderPass();

private:
	VkRenderPass renderPass;
};

#endif // RENDERPASS_H
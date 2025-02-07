#ifndef FRAMEBUFFERS_H
#define FRAMEBUFFERS_H

#include "core/Device.h"
#include "graphics/SwapChain.h"
#include "graphics/ImageViews.h"
#include "graphics/RenderPass.h"

#include <vulkan/vulkan.h>

// The attachments specified during render pass creation are bound by wrapping them into a VkFramebuffer object.
// A framebuffer object references all of the VkImageView objects that represent the attachments.
class FrameBuffers
{
public:
	void initialize(SwapChain* pSwapChain, ImageViews* pImageViews, RenderPass* pRenderPass);
    void cleanup();
    const std::vector<VkFramebuffer> getSwapChainFramebuffers();

private:
	// We have to create a framebuffer for all of the images in the swap chain and use the one that corresponds to the retrieved image at drawing time.
	std::vector<VkFramebuffer> swapChainFramebuffers;
};

#endif // FRAMEBUFFERS_H
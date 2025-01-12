#ifndef FRAMEBUFFERS_H
#define FRAMEBUFFERS_H

#include "core/Device.h"
#include "graphics/SwapChain.h"
#include "graphics/ImageViews.h"
#include "graphics/RenderPass.h"

#include <vulkan/vulkan.h>

class FrameBuffers
{
public:
	void initialize(Device* pdevice, SwapChain* pSwapChain, ImageViews* pImageViews, RenderPass* pRenderPass);
    void cleanup(Device* pdevice);
    const std::vector<VkFramebuffer> getSwapChainFramebuffers();

private:
	// We have to create a framebuffer for all of the images in the swap chain and use the one that corresponds to the retrieved image at drawing time.
	std::vector<VkFramebuffer> swapChainFramebuffers;
};

#endif // FRAMEBUFFERS_H
#include "graphics/FrameBuffers.h"

void FrameBuffers::initialize(SwapChain* pSwapChain, ImageViews* pImageViews, RenderPass* pRenderPass) {
    size_t swapChainImageViewsSize = pImageViews->getSwapChainImageViews().size();
    VkExtent2D swapChainExtent = pSwapChain->getSwapChainExtent();

    swapChainFramebuffers.resize(swapChainImageViewsSize); // Resizing the container to hold all of the framebuffers

    // Iterate through the image views and create framebuffers from them
    for (size_t i = 0; i < swapChainImageViewsSize; i++) {
        VkImageView attachments[] = {
            pImageViews->getSwapChainImageViews()[i]
        };

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = pRenderPass->getRenderPass(); // they use the same number and type of attachments.
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments = attachments;
        framebufferInfo.width = swapChainExtent.width;
        framebufferInfo.height = swapChainExtent.height;
        framebufferInfo.layers = 1; // Our swap chain images are single images, so the number of layers is 1

        if (vkCreateFramebuffer(RendererContext::getInstance().pdevice->getLogicalDevice(), &framebufferInfo, nullptr, &swapChainFramebuffers[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create framebuffer!");
        }
    }
}

void FrameBuffers::cleanup() {
    for (auto framebuffer : swapChainFramebuffers) {
        vkDestroyFramebuffer(RendererContext::getInstance().pdevice->getLogicalDevice(), framebuffer, nullptr);
    }
}

const std::vector<VkFramebuffer> FrameBuffers::getSwapChainFramebuffers() {
    return swapChainFramebuffers;
}
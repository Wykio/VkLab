#include "graphics/RenderPass.h"


void RenderPass::initialize(Device* pdevice, SwapChain* pswapchain) {
	VkAttachmentDescription colorAttachment{}; // Single color attachement for now
	colorAttachment.format = pswapchain->getSwapChainImageFormat();
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR; // Clear the values to a constant at the start
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE; // Rendered contents will be stored in memory and can be read later

	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE; // Our application won’t do anything with the stencil buffer
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

	// Images need to be transitioned to specific layouts that are suitable for the operation that they’re going to be involved in next.
	// InitialLayout specifies which layout the image will have before the render pass begins.
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	// FinalLayout specifies the layout to automatically transition to when the render pass finishes.
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR; // Images to be presented in the swap chain

	VkAttachmentReference colorAttachmentRef{};
	colorAttachmentRef.attachment = 0; // Index 0 refers to our single colorAttachment
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL; // We intend to use the attachment to function as a color buffer

	VkSubpassDescription subpass{};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS; // We have to be explicit about this being a graphics subpass
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef; // Directly referenced from the fragment shader with the layout(location = 0) out vec4 outColor directive

	VkRenderPassCreateInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = 1;
	renderPassInfo.pAttachments = &colorAttachment;
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;

	if (vkCreateRenderPass(pdevice->getLogicalDevice(), &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS) {
		throw std::runtime_error("failed to create render pass!");
	}
}

void RenderPass::cleanup(Device* pdevice) {
	vkDestroyRenderPass(pdevice->getLogicalDevice(), renderPass, nullptr);
}
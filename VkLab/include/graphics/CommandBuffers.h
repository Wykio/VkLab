#ifndef COMMANDBUFFERS_H
#define COMMANDBUFFERS_H

#include "core/Constant.h"
#include "graphics/SwapChain.h"
#include "graphics/RenderPass.h"
#include "graphics/Pipeline.h"
#include "graphics/FrameBuffers.h"
#include "graphics/CommandPools.h"
#include "graphics/Vertex.h"

#include <vulkan/vulkan.h>

class CommandBuffers
{
public:
	void initialize(Device* pdevice, CommandPools* pCommandPool);
	std::vector<VkCommandBuffer> getCommandBuffers();
	VkCommandBuffer getCommandBuffer(const int index);
	VkCommandBuffer* getCommandBufferPtr(const int index); // not a very good practice :/

private:
	std::vector<VkCommandBuffer> commandBuffers;
};

void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex, SwapChain* pSwapChain, RenderPass* pRenderPass, Pipeline* pPipeline, FrameBuffers* pFrameBuffer, VertexBuffer* pvertexbuffer);

#endif // COMMANDBUFFERS_H
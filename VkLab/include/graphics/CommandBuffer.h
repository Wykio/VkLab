#ifndef COMMANDBUFFER_H
#define COMMANDBUFFER_H

#include "graphics/SwapChain.h"
#include "graphics/RenderPass.h"
#include "graphics/Pipeline.h"
#include "graphics/FrameBuffer.h"
#include "graphics/CommandPool.h"

#include <vulkan/vulkan.h>

class CommandBuffer
{
public:
	void initialize(Device* pdevice, CommandPool* pCommandPool);
    void recordCommandBuffer(uint32_t imageIndex, SwapChain* pSwapChain, RenderPass* pRenderPass, Pipeline* pPipeline, FrameBuffer* pFrameBuffer);
    VkCommandBuffer getCommandBuffer();
	VkCommandBuffer* getCommandBufferPtr(); // not a very good practice :/

private:
	VkCommandBuffer commandBuffer;
};

#endif // COMMANDBUFFER_H
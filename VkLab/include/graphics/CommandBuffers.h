#ifndef COMMANDBUFFERS_H
#define COMMANDBUFFERS_H

#include "core/Constant.h"
#include "graphics/SwapChain.h"
#include "graphics/RenderPass.h"
#include "graphics/DescriptorSet.h"
#include "graphics/Pipeline.h"
#include "graphics/FrameBuffers.h"
//#include "graphics/CommandPools.h"
//#include "graphics/BufferManager.h"

#include <vulkan/vulkan.h>
#include <vector>

class SwapChain;
class RenderPass;
class DescriptorSet;
class Pipeline;
class FrameBuffers;
class CommandPools;
class BufferManager;

class CommandBuffers
{
public:
	void initialize(CommandPools* pCommandPool);
	std::vector<VkCommandBuffer> getCommandBuffers();
	VkCommandBuffer getCommandBuffer(const int index);
	VkCommandBuffer* getCommandBufferPtr(const int index); // not a very good practice :/

private:
	std::vector<VkCommandBuffer> commandBuffers;
};

void recordCommandBuffer(
    VkCommandBuffer commandBuffer,
    uint32_t currentFrame,
    uint32_t imageIndex,
    SwapChain* pSwapChain,
    RenderPass* pRenderPass,
    DescriptorSet* pDescriptorSet,
    Pipeline* pPipeline,
    FrameBuffers* pFrameBuffer,
    BufferManager* pvertexbuffer
);

VkCommandBuffer beginSingleTimeCommands(VkCommandPool commandPool);
void endSingleTimeCommands(VkCommandPool commandPool, VkCommandBuffer commandBuffer);

#endif // COMMANDBUFFERS_H
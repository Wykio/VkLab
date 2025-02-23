#ifndef PIPELINE_H
#define PIPELINE_H

#include "core/Device.h"
#include "graphics/RenderPass.h"
#include "graphics/BufferManager.h"
#include "utils/shaderUtils.h"

#include <iostream>

class DescriptorSet;

// The Pipeline is assembled with the renderpass infos and shaders
class Pipeline
{
public:
	void initialize(RenderPass* prenderpass, DescriptorSet* pdescriptorset);
	void cleanup();
	VkPipelineLayout getPipelineLayout();
	VkPipeline getGraphicsPipeline();

private:
	VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
	VkPipeline graphicsPipeline;
};

#endif // PIPELINE_H
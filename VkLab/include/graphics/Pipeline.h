#ifndef PIPELINE_H
#define PIPELINE_H

#include "core/Device.h"
#include "graphics/RenderPass.h"
#include "graphics/VertexBuffer.h"
#include "utils/shaderUtils.h"

#include <iostream>

class Pipeline
{
public:
	void initialize(Device* pdevice, RenderPass* prenderpass);
	void cleanup(Device* pdevice);
	VkPipeline getGraphicsPipeline();

private:
	VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
	VkPipeline graphicsPipeline;
};

#endif // PIPELINE_H
#ifndef PIPELINE_H
#define PIPELINE_H

#include "core/Device.h"
#include "utils/shaderUtils.h"

#include <iostream>

class Pipeline
{
public:
	void initialize(Device* pdevice);
	void cleanup(Device* pdevice);

private:
	VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
};

#endif // PIPELINE_H
#ifndef COMMANDPOOLS_H
#define COMMANDPOOLS_H

#include "core/Device.h"

#include <vulkan/vulkan.h>

class Device;

class CommandPools
{
public:
	void initialize();
	void cleanup();
	VkCommandPool getDrawCommandPool();
	VkCommandPool getTransferCommandPool();

private:
	VkCommandPool drawCommandPool;
	VkCommandPool transferCommandPool;
};

#endif // COMMANDPOOLS_H
#ifndef COMMANDPOOLS_H
#define COMMANDPOOLS_H

#include "core/Device.h"

#include <vulkan/vulkan.h>

class CommandPools
{
public:
	void initialize(Device* pdevice);
	void cleanup(Device* pdevice);
	VkCommandPool getDrawCommandPool();
	VkCommandPool getTransferCommandPool();

private:
	VkCommandPool drawCommandPool;
	VkCommandPool transferCommandPool;
};

#endif // COMMANDPOOLS_H
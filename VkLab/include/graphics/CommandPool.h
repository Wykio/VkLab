#ifndef COMMANDPOOL_H
#define COMMANDPOOL_H

#include "core/Device.h"

#include <vulkan/vulkan.h>

class CommandPool
{
public:
	void initialize(Device* pdevice, VkSurfaceKHR* psurface);
	void cleanup(Device* pdevice);
	VkCommandPool getCommandPool();

private:
	VkCommandPool commandPool;
};

#endif // COMMANDPOOL_H
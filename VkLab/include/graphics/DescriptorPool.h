#ifndef DESCRIPTOR_POOL_H
#define DESCRIPTOR_POOL_H

#include "core/Device.h"
#include "core/Constant.h"	

// Descriptor sets can’t be created directly, they must be allocated from a pool like command buffers.
// The Descriptor Pool is a container for allocating multiple Descriptor Sets
// Since Vulkan does not allow you to allocate a Descriptor Set directly,
// you must first create a Descriptor Pool and then extract the Descriptor Sets from it.
class DescriptorPool
{
public:
	void initialize(Device* pdevice);
	void cleanup(Device* pdevice);
	VkDescriptorPool getDescriptorPool();

private:
	VkDescriptorPool descriptorPool;
};

#endif // DESCRIPTOR_POOL_H
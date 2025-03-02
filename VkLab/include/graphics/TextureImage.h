#ifndef TEXTURE_IMAGE_H
#define TEXTURE_IMAGE_H

#include "utils/Buffer.h"
#include "utils/Image.h"
#include "utils/CommandBuffersUtils.h"

#include <vulkan/vulkan.h>
#include <stdexcept>

class TextureImage
{
public:
	void initialize(CommandPools commandPools);
    void cleanup();

private:
    VkImage textureImage;
    VkDeviceMemory textureImageMemory;
};

void transitionImageLayout(VkCommandPool commandPool, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);

#endif // TEXTURE_IMAGE_H
#ifndef TEXTURE_IMAGE_H
#define TEXTURE_IMAGE_H

#include "utils/Buffer.h"
#include "utils/Image.h"

#include <vulkan/vulkan.h>
#include <stdexcept>

class TextureImage
{
public:
	void initialize();

private:
    VkImage textureImage;
    VkDeviceMemory textureImageMemory;
};

#endif // TEXTURE_IMAGE_H
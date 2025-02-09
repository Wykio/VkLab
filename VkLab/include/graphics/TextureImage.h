#ifndef TEXTURE_IMAGE_H
#define TEXTURE_IMAGE_H

#include "utils/Buffer.h"

#include <vulkan/vulkan.h>
#include <stdexcept>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

class TextureImage
{
public:
	void initialize();

private:

};

void TextureImage::initialize() {
    auto pdevice = RendererContext::getInstance().pdevice;

    // Loading the image with stb_image library
    int texWidth, texHeight, texChannels;
    stbi_uc* pixels = stbi_load("textures/texture.jpg", &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    VkDeviceSize imageSize = texWidth * texHeight * 4;

    if (!pixels) {
        throw std::runtime_error("failed to load texture image!");
    }

    // Create staging buffer to copy the pixels to it
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;

    createBuffer(
        pdevice,
        imageSize,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        stagingBuffer,
        stagingBufferMemory
    );

    // Copy the pixel values that we got from the image loading library to the buffer
    void* data;
    vkMapMemory(pdevice->getLogicalDevice(), stagingBufferMemory, 0, imageSize, 0, &data);
    memcpy(data, pixels, static_cast<size_t>(imageSize));
    vkUnmapMemory(pdevice->getLogicalDevice(), stagingBufferMemory);
    
}

#endif // TEXTURE_IMAGE_H
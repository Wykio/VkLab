#ifndef IMAGE_H
#define IMAGE_H

#include "core/Device.h"
#include "utils/Buffer.h"

#include "vulkan/vulkan.h"

inline void createImage(
    Device* pdevice,
    uint32_t width,
    uint32_t height,
    VkFormat format,
    VkImageTiling tiling,
    VkImageUsageFlags usage,
    VkMemoryPropertyFlags properties,
    VkImage& image,
    VkDeviceMemory& imageMemory
) {
    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D; // Tells Vulkan with what kind of coordinate system the texels in the image are going to be addressed (3D for voxel volumes)
    imageInfo.extent.width = width;
    imageInfo.extent.height = height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.format = format;
    imageInfo.tiling = tiling;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED; // Not usable by the GPU and the very first transition will discard the texels
    imageInfo.usage = usage;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE; // The image will only be used by one queue family : the one that supports graphics(and therefore also) transfer operations

    if (vkCreateImage(pdevice->getLogicalDevice(), &imageInfo, nullptr, &image) != VK_SUCCESS) {
        throw std::runtime_error("failed to create image!");
    }

    // Allocating memory for an image works in exactly the same way as allocating memory for a buffer
    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(pdevice->getLogicalDevice(), image, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(pdevice, memRequirements.memoryTypeBits, properties);

    if (vkAllocateMemory(pdevice->getLogicalDevice(), &allocInfo, nullptr, &imageMemory) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate image memory!");
    }

    vkBindImageMemory(pdevice->getLogicalDevice(), image, imageMemory, 0);
}

#endif IMAGE_H
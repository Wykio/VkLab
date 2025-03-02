#include "graphics/TextureImage.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

// TODO ! Try to experiment with this by creating a setupCommandBuffer that the helper functions record commands into,
// and add a flushSetupCommands to execute the commands that have been recorded so far.It’s best to do this after the texture mapping works
// to check if the texture resources are still set up correctly.

void TextureImage::initialize(CommandPools commandPools) {
    auto pdevice = RendererContext::getInstance().pdevice;
    auto logicalDevice = pdevice->getLogicalDevice();
    auto transferCommandPool = commandPools.getTransferCommandPool();
    auto drawCommandPool = commandPools.getDrawCommandPool();

    // Loading the image with stb_image library
    int texWidth, texHeight, texChannels;
    stbi_uc* pixels = stbi_load("textures/statue.jpg", &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
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
    vkMapMemory(logicalDevice, stagingBufferMemory, 0, imageSize, 0, &data);
    memcpy(data, pixels, static_cast<size_t>(imageSize));
    vkUnmapMemory(logicalDevice, stagingBufferMemory);

    stbi_image_free(pixels);

    createImage(
        pdevice,
        texWidth,
        texHeight,
        VK_FORMAT_R8G8B8A8_SRGB,
        VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        textureImage,
        textureImageMemory
    );

    // Transition the texture image to VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
    transitionImageLayout(drawCommandPool, textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

    // Execute the buffer to image copy operation
    copyBufferToImage(pdevice, transferCommandPool, stagingBuffer, textureImage, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));

    // Transition the texture image to start sampling in the shader (for shader access)
    transitionImageLayout(drawCommandPool, textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    vkDestroyBuffer(logicalDevice, stagingBuffer, nullptr);
    vkFreeMemory(logicalDevice, stagingBufferMemory, nullptr);
}

void TextureImage::cleanup() {
    auto logicalDevice = RendererContext::getInstance().pdevice->getLogicalDevice();

    vkDestroyImage(logicalDevice, textureImage, nullptr);
    vkFreeMemory(logicalDevice, textureImageMemory, nullptr);
}

// Handle layout transitions (vkCmdCopyBufferToImage needs the image to be in the right layout first)
void transitionImageLayout(VkCommandPool commandPool, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout) {
    auto pdevice = RendererContext::getInstance().pdevice;
    auto logicalDevice = pdevice->getLogicalDevice();
    uint32_t graphicsQueueFamily = findQueueFamilies(pdevice->getPhysicalDevice()).graphicsFamily.value();

    VkCommandBuffer commandBuffer = beginSingleTimeCommands(logicalDevice, commandPool);

    // Using an image memory barrier to transition image layouts and transfer queue family ownership
    // Memory barrier are used to syncronise operations on a ressource
    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout; // VK_IMAGE_LAYOUT_UNDEFINED if we don't care about the existing content
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = graphicsQueueFamily; // index of the transfer queue, else VK_QUEUE_FAMILY_IGNORED
    barrier.dstQueueFamilyIndex = graphicsQueueFamily;
    // The image and subresourceRange specify the image that is affected and the specific part of the image
    // Our image is not an array and does not have mipmapping levels, so only one level and layer are specified
    barrier.image = image;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    VkPipelineStageFlags sourceStage;
    VkPipelineStageFlags destinationStage;

    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        barrier.srcAccessMask = 0; // We trust Vulkan implicite syncronization. We need to be sure that CPU writings are visible for the GPU.
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        // Since the writes don’t have to wait on anything, we may specify an empty access mask and the earliest possible pipeline stage
        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT; //  For the pre-barrier operations
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT; // Transfer writes must occur in the pipeline transfer stage
        // VK_PIPELINE_STAGE_TRANSFER_BIT is not a real stage within the graphics and compute pipelines. It is more of a pseudo-stage where transfers happen
        // See the documentation (https://docs.vulkan.org/spec/latest/chapters/synchronization.html#VkPipelineStageFlagBits)
        // for more information and other examples of pseudo-stages.
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    else {
        throw std::invalid_argument("unsupported layout transition!");
    }

    // We must specify which types of operations that involve the resource must happen before the barrier,
    // and which operations that involve the resource must wait on the barrier
    vkCmdPipelineBarrier(
        commandBuffer,
        sourceStage, destinationStage,
        0,
        0, nullptr,
        0, nullptr,
        1, &barrier
    );

    endSingleTimeCommands(logicalDevice, pdevice->getGraphicsQueue(), commandPool, commandBuffer);
}
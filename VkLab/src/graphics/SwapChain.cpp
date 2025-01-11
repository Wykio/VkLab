#include "graphics/SwapChain.h"

void SwapChain::initialize(Device* pdevice, VkSurfaceKHR* psurface, GLFWwindow* window) {
    VkPhysicalDevice physicalDevice = pdevice->getPhysicalDevice();
    VkDevice logicalDevice = pdevice->getLogicalDevice();

    SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice, psurface);

    VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
    VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities, window);

    // How many images we would like to have in the swap chain
    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;

    // Make sure to not exceed the maximum number of images
    if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = *psurface;

    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1; // always 1 unless you are developing a stereoscopic 3D application.
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    // Specify how to handle swap chain images that will be used across multiple queue families (if graphicsFamily and presentFamily are not the same).
    QueueFamilyIndices indices = findQueueFamilies(physicalDevice, psurface);
    uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

    if (indices.graphicsFamily != indices.presentFamily) {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0; // Optional
        createInfo.pQueueFamilyIndices = nullptr; // Optional
    }

    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR; // Specifies if the alpha channel should be used for blending with other windows
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE; // Means that we don’t care about the color of pixels that are obscured
    createInfo.oldSwapchain = VK_NULL_HANDLE; // We assume that we’ll only ever create one swap chain

    if (vkCreateSwapchainKHR(logicalDevice, &createInfo, nullptr, &swapChain) != VK_SUCCESS) {
        throw std::runtime_error("failed to create swap chain!");
    }

    // Remember that we only specified a minimum number of images in the swap chain, so the implementation is allowed to create a swap chain with more.
    vkGetSwapchainImagesKHR(logicalDevice, swapChain, &imageCount, nullptr); 
    swapChainImages.resize(imageCount); // That’s why resize the container first
    vkGetSwapchainImagesKHR(logicalDevice, swapChain, &imageCount, swapChainImages.data());

    swapChainImageFormat = surfaceFormat.format;
    swapChainExtent = extent;
}

void SwapChain::cleanup(Device* device) {
    if (swapChain != VK_NULL_HANDLE) {
        vkDestroySwapchainKHR(device->getLogicalDevice(), swapChain, nullptr);
    }
}

const VkSwapchainKHR SwapChain::getSwapChain() {
    return swapChain;
}

const std::vector<VkImage> SwapChain::getSwapChainImages() {
    return swapChainImages;
}

const VkFormat SwapChain::getSwapChainImageFormat() {
    return swapChainImageFormat;
}

const VkExtent2D SwapChain::getSwapChainExtent() {
    return swapChainExtent;
}

// Select VK_FORMAT_B8G8R8A8_SRGB format and VK_COLOR_SPACE_SRGB_NONLINEAR_KHR if available
VkSurfaceFormatKHR SwapChain::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
    for (const auto& availableFormat : availableFormats) {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return availableFormat;
        }
    }

    return availableFormats[0];
}

// Select VK_PRESENT_MODE_MAILBOX_KHR if available 
VkPresentModeKHR SwapChain::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
    for (const auto& availablePresentMode : availablePresentModes) {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return availablePresentMode;
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

// Choose the resolution of the swap chain images
VkExtent2D SwapChain::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, GLFWwindow* window) {
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        return capabilities.currentExtent;
    }
    else { // In that case we’ll pick the resolution that best matches the window within the minImageExtent and maxImageExtent bounds.
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);

        VkExtent2D actualExtent = {
            static_cast<uint32_t>(width),
            static_cast<uint32_t>(height)
        };

        actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

        return actualExtent;
    }
}
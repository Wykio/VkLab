#ifndef RENDERER_H
#define RENDERER_H

#include "core/VulkanInstance.h"
#include "core/Device.h"
#include "core/Swapchain.h"
#include "core/ImageViews.h"
#include "core/Pipeline.h"
#include "graphics/RenderPass.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>
#include <optional>
#include <set>

class Renderer {
public:
    void run();

private:
    void initWindow();
    void initVulkan();
    void mainLoop();
    void cleanup();

    // Vulkan-specific methods
    void createSurface();

    // Vulkan resources
    GLFWwindow* window;

    VulkanInstance r_instance;

    DebugMessenger r_debugMessenger;
    VkSurfaceKHR surface = VK_NULL_HANDLE;

    Device r_device;

    VkQueue graphicsQueue = VK_NULL_HANDLE;
    VkQueue presentQueue = VK_NULL_HANDLE;

    SwapChain r_swapchain;

    ImageViews r_imageviews;

    Pipeline r_pipeline;

    RenderPass r_renderpass;
};

#endif // RENDERER_H
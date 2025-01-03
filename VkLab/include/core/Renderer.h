#ifndef RENDERER_H
#define RENDERER_H

#include "core/VulkanInstance.h"
#include "core/Device.h"
#include "core/Swapchain.h"

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

    Swapchain r_swapchain;
};

#endif // RENDERER_H
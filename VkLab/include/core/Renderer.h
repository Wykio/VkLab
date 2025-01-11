#ifndef RENDERER_H
#define RENDERER_H

#include "core/Constant.h"
#include "core/VulkanInstance.h"
#include "core/Device.h"
#include "graphics/Swapchain.h"
#include "graphics/ImageViews.h"
#include "graphics/Pipeline.h"
#include "graphics/RenderPass.h"
#include "graphics/FrameBuffer.h"
#include "graphics/CommandPool.h"
#include "graphics/CommandBuffers.h"

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
    void drawFrame();
    void createSyncObjects();

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
    FrameBuffer r_framebuffer;
    CommandPool r_commandpool;

    CommandBuffers r_commandbuffers;

    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;

    uint32_t currentFrame = 0;
};

#endif // RENDERER_H
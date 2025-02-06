#ifndef RENDERER_H
#define RENDERER_H

#include "core/RendererContext.h" // Include singleton
#include "core/Constant.h"
#include "core/VulkanInstance.h"
#include "core/Device.h"
#include "graphics/Swapchain.h"
#include "graphics/ImageViews.h"
#include "graphics/Pipeline.h"
#include "graphics/RenderPass.h"
#include "graphics/FrameBuffers.h"
#include "graphics/CommandPools.h"
#include "graphics/CommandBuffers.h"
#include "graphics/BufferManager.h"
#include "graphics/DescriptorSet.h"
#include "graphics/DescriptorPool.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>
#include <optional>
#include <set>

class Renderer {
public:
    void run();

    bool framebufferResized = false; // Handle resize explicitly

private:
    void initWindow();
    void initVulkan();
    void mainLoop();
    void cleanup();

    // Vulkan-specific methods
    void createSurface();
    void drawFrame();
    void createSyncObjects();

    void cleanupSwapChain();
    void recreateSwapChain();

    // Vulkan resources
    GLFWwindow* window;

    VulkanInstance r_instance;

    DebugMessenger r_debugMessenger;

    Device r_device;

    SwapChain r_swapchain;
    ImageViews r_imageviews;
    Pipeline r_pipeline;
    RenderPass r_renderpass;
    DescriptorPool r_descriptorpool;
    DescriptorSet r_descriptorset;
    FrameBuffers r_framebuffer;
    CommandPools r_commandpools;
    BufferManager r_buffermanager;
    CommandBuffers r_commandbuffers;

    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;

    uint32_t currentFrame = 0;
};

static void framebufferResizeCallback(GLFWwindow* window, int width, int height);

#endif // RENDERER_H
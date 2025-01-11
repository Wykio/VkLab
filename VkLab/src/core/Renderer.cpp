#include "Core/Renderer.h"

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

void Renderer::run() {
    initWindow();
    initVulkan();
    mainLoop();
    cleanup();
}

// Initializes a GLFW window without an OpenGL context.
void Renderer::initWindow() {
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    window = glfwCreateWindow(WIDTH, HEIGHT, "Ksoss renderer", nullptr, nullptr);
}

// Initializes Vulkan components needed for the application.
void Renderer::initVulkan() {
    r_instance.initialize();

    if (enableValidationLayers) {
        r_debugMessenger.initialize(r_instance.getInstance());
    }

    createSurface();

    r_device.initialize(r_instance.getInstance(), &surface, &graphicsQueue, &presentQueue);
    r_swapchain.initialize(&r_device, &surface, window);
    r_imageviews.initialize(&r_device, &r_swapchain);
    r_renderpass.initialize(&r_device, &r_swapchain);
    r_pipeline.initialize(&r_device, &r_renderpass);
    r_framebuffer.initialize(&r_device, &r_swapchain, &r_imageviews, &r_renderpass);
    r_commandpool.initialize(&r_device, &surface);
    r_commandbuffer.initialize(&r_device, &r_commandpool);

    createSyncObjects();
}

// Runs the main event loop of the application.
void Renderer::mainLoop() {
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        drawFrame();
    }

    // We should wait for the logical device to finish operations before exiting mainLoop and destroying the window
    vkDeviceWaitIdle(r_device.getLogicalDevice());
}

// Cleans up all Vulkan and GLFW resources.
void Renderer::cleanup() {
    vkDestroySemaphore(r_device.getLogicalDevice(), imageAvailableSemaphore, nullptr);
    vkDestroySemaphore(r_device.getLogicalDevice(), renderFinishedSemaphore, nullptr);
    vkDestroyFence(r_device.getLogicalDevice(), inFlightFence, nullptr);

    r_commandpool.cleanup(&r_device);
    r_framebuffer.cleanup(&r_device);
    r_pipeline.cleanup(&r_device);
    r_renderpass.cleanup(&r_device);
    r_imageviews.cleanup(&r_device);
    r_swapchain.cleanup(&r_device);
    r_device.cleanup();

    if (enableValidationLayers) {
        r_debugMessenger.cleanup(r_instance.getInstance());
    }

    vkDestroySurfaceKHR(r_instance.getInstance(), surface, nullptr);
    r_instance.cleanup();

    glfwDestroyWindow(window);
    glfwTerminate();
}

// Creates a Vulkan surface for the GLFW window.
void Renderer::createSurface() {
    if (glfwCreateWindowSurface(r_instance.getInstance(), window, nullptr, &surface) != VK_SUCCESS) {
        throw std::runtime_error("failed to create window surface!");
    }
}

void Renderer::drawFrame() {
    // At a high level, rendering a frame in Vulkan consists of a common set of steps:
    // - Wait for the previous frame to finish
    // - Acquire an image from the swap chain
    // - Record a command buffer which draws the scene onto that image
    // - Submit the recorded command buffer
    // - Present the swap chain image
    // While we will expand the drawing function in later chapters, for now this is the core of our render loop.

    // A core design philosophy in Vulkan is that synchronization of execution on the GPU is explicit.


    // Note that in this code snippet, both calls to vkQueueSubmit() return immediately, only the GPU wait.

    // if the host(CPU) needs to know when the GPU has finished something, we use a fence.
    // In general, it is preferable to not block the host unless necessary.

    //Because we re-record the command buffer every frame, we cannot record the next frame’s work to the command buffer
    // until the current frame has finished executing, as we don’t want to overwrite the current contents of
    // the command buffer while the GPU is using it.

    // - Wait for the previous frame to finish
    vkWaitForFences(r_device.getLogicalDevice(), 1, &inFlightFence, VK_TRUE, UINT64_MAX);
    vkResetFences(r_device.getLogicalDevice(), 1, &inFlightFence);
    
    uint32_t imageIndex;
    // Recall that the swap chain is an extension feature, so we must use a function with the vk*KHR naming convention
    vkAcquireNextImageKHR(r_device.getLogicalDevice(), r_swapchain.getSwapChain(), UINT64_MAX, imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);

    vkResetCommandBuffer(r_commandbuffer.getCommandBuffer(), /*VkCommandBufferResetFlagBits*/ 0);
    // Record the commands we want.
    r_commandbuffer.recordCommandBuffer(imageIndex, &r_swapchain, &r_renderpass, &r_pipeline, &r_framebuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = { imageAvailableSemaphore }; // We want to wait with writing colors to the image until it’s available
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT }; // so we’re specifying the stage of the graphics pipeline that writes to the color attachment
    
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores; // Each entry in the waitStages array corresponds to the semaphore with the same index in pWaitSemaphores
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = r_commandbuffer.getCommandBufferPtr(); // Specify which command buffers to actually submit for execution

    VkSemaphore signalSemaphores[] = { renderFinishedSemaphore }; // Specify which command buffers to actually submit for execution
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores; // Specify which semaphores to signal once the command buffer(s) have finished execution

    if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFence) != VK_SUCCESS) {
        throw std::runtime_error("failed to submit draw command buffer!");
    }

    // The last step of drawing a frame is submitting the result back to the swap chain to have it eventually show up on the screen
    // Presentation is configured through a VkPresentInfoKHR structure
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores; // specify which semaphores to wait on before presentation can happen

    VkSwapchainKHR swapChains[] = { r_swapchain.getSwapChain() }; // specify the swap chains to present images to and the index of the image for each swap chain.
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;
    presentInfo.pResults = nullptr; // Optional Allows you to specify an array of VkResult values to check for every individual swap chain if presentation was successful.
    // With 1 swapChain, you can simply use the return value of the vkQueuePresentKHR function.

    // Submits the request to present an image to the swap chain.
    vkQueuePresentKHR(presentQueue, &presentInfo);
}

void Renderer::createSyncObjects() {
    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT; // To be signaled the first time we wait for it

    if (vkCreateSemaphore(r_device.getLogicalDevice(), &semaphoreInfo, nullptr, &imageAvailableSemaphore) != VK_SUCCESS ||
        vkCreateSemaphore(r_device.getLogicalDevice(), &semaphoreInfo, nullptr, &renderFinishedSemaphore) != VK_SUCCESS ||
        vkCreateFence(r_device.getLogicalDevice(), &fenceInfo, nullptr, &inFlightFence) != VK_SUCCESS) {
        throw std::runtime_error("failed to create semaphores!");
    }
}
#include "Core/Renderer.h"

// Main function
void Renderer::run() {
    initWindow();
    initVulkan();
    mainLoop();
    cleanup();
}

// Initializes a GLFW window without an OpenGL context
void Renderer::initWindow() {
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    window = glfwCreateWindow(WIDTH, HEIGHT, "Renderer", nullptr, nullptr);
    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
}

// Initializes Vulkan components needed for the application
void Renderer::initVulkan() {
    r_instance.initialize();

    if (enableValidationLayers) {
        r_debugMessenger.initialize(r_instance.getInstance());
    }

    createSurface();

    r_device.initialize(r_instance.getInstance());
    RendererContext::getInstance().pdevice = &r_device;
    r_swapchain.initialize(window);
    r_imageviews.initialize(&r_swapchain);
    r_renderpass.initialize(&r_swapchain);
    r_descriptorpool.initialize();
    r_descriptorset.initialize();
    r_pipeline.initialize(&r_renderpass, &r_descriptorset);
    r_framebuffer.initialize(&r_swapchain, &r_imageviews, &r_renderpass);
    r_commandpools.initialize();
    r_buffermanager.initialize(&r_commandpools);
    r_descriptorset.allocate(&r_descriptorpool, &r_buffermanager); // UBO must be set
    r_commandbuffers.initialize(&r_commandpools);

    createSyncObjects();
}

// Runs the main event loop of the application.
void Renderer::mainLoop() {
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        drawFrame();
    }

    // We should wait for the logical device to finish operations before exiting mainLoop and destroying the window
    vkDeviceWaitIdle(RendererContext::getInstance().pdevice->getLogicalDevice());
}

// Cleans up all Vulkan and GLFW resources.
void Renderer::cleanup() {
    auto& context = RendererContext::getInstance();

    cleanupSwapChain();

    r_buffermanager.cleanup();
    r_descriptorpool.cleanup();
    r_descriptorset.cleanup();
    r_pipeline.cleanup();
    r_renderpass.cleanup();

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroySemaphore(context.pdevice->getLogicalDevice(), renderFinishedSemaphores[i], nullptr);
        vkDestroySemaphore(context.pdevice->getLogicalDevice(), imageAvailableSemaphores[i], nullptr);
        vkDestroyFence(context.pdevice->getLogicalDevice(), inFlightFences[i], nullptr);
    }

    r_commandpools.cleanup();
    context.pdevice->cleanup();

    if (enableValidationLayers) {
        r_debugMessenger.cleanup(r_instance.getInstance());
    }

    vkDestroySurfaceKHR(r_instance.getInstance(), context.surface, nullptr);
    r_instance.cleanup();

    glfwDestroyWindow(window);
    glfwTerminate();
}

// Creates a Vulkan surface for the GLFW window.
void Renderer::createSurface() {
    if (glfwCreateWindowSurface(r_instance.getInstance(), window, nullptr, &RendererContext::getInstance().surface) != VK_SUCCESS) {
        throw std::runtime_error("failed to create window surface!");
    }

    std::cout << "Surface created and stored in RendererContext.\n";
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
    auto& context = RendererContext::getInstance();

    // - Wait for the previous frame to finish
    vkWaitForFences(context.pdevice->getLogicalDevice(), 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);
    
    uint32_t imageIndex;
    // Recall that the swap chain is an extension feature, so we must use a function with the vk*KHR naming convention
    VkResult result = vkAcquireNextImageKHR(context.pdevice->getLogicalDevice(), r_swapchain.getSwapChain(), UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);
    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        recreateSwapChain();
    } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) { // VK_SUBOPTIMAL_KHR is ok because we still have an image
        throw std::runtime_error("failed to acquire swap chain image!");
    }

    // Generate a new transformation every frame to make the geometry spin around
    r_buffermanager.updateUniformBuffer(r_swapchain, currentFrame);

    // Only reset the fence if we are submitting work (avoid Deadlock)
    vkResetFences(context.pdevice->getLogicalDevice(), 1, &inFlightFences[currentFrame]);

    vkResetCommandBuffer(r_commandbuffers.getCommandBuffer(currentFrame), /*VkCommandBufferResetFlagBits*/ 0);
    recordCommandBuffer(
        r_commandbuffers.getCommandBuffer(currentFrame),
        currentFrame,
        imageIndex,
        &r_swapchain,
        &r_renderpass,
        &r_descriptorset,
        &r_pipeline,
        &r_framebuffer,
        &r_buffermanager
    );

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = { imageAvailableSemaphores[currentFrame] }; // We want to wait with writing colors to the image until it’s available
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT }; // so we’re specifying the stage of the graphics pipeline that writes to the color attachment
    
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores; // Each entry in the waitStages array corresponds to the semaphore with the same index in pWaitSemaphores
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = r_commandbuffers.getCommandBufferPtr(currentFrame); // Specify which command buffers to actually submit for execution

    VkSemaphore signalSemaphores[] = { renderFinishedSemaphores[currentFrame]}; // Specify which command buffers to actually submit for execution
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores; // Specify which semaphores to signal once the command buffer(s) have finished execution

    if (vkQueueSubmit(context.pdevice->getGraphicsQueue(), 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS) {
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
    result = vkQueuePresentKHR(context.pdevice->getPresentQueue(), &presentInfo);
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized) { // Because we want the best possible result.
        framebufferResized = false; // Ensure that the semaphores are in a consistent state, otherwise a signaled semaphore may never be properly waited upon
        recreateSwapChain();
    }
    else if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to present swap chain image!");
    }

    // advance to the next frame every time
    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT; // By using the modulo (%) operator, we ensure that the frame index loops around after every MAX_FRAMES_IN_FLIGHT enqueued frames.
}

void Renderer::createSyncObjects() {
    auto& context = RendererContext::getInstance();

    imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT; // To be signaled the first time we wait for it

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        if (vkCreateSemaphore(context.pdevice->getLogicalDevice(), &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
            vkCreateSemaphore(context.pdevice->getLogicalDevice(), &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
            vkCreateFence(context.pdevice->getLogicalDevice(), &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS) {

            throw std::runtime_error("failed to create synchronization objects for a frame!");
        }
    }
}

void Renderer::cleanupSwapChain() {
    r_framebuffer.cleanup();
    r_imageviews.cleanup();
    r_swapchain.cleanup();
}

void Renderer::recreateSwapChain() {
    // Avoid crash when minimized
    int width = 0, height = 0;
    glfwGetFramebufferSize(window, &width, &height); // Retrieve frame buffer size
    while (width == 0 || height == 0) {
        glfwGetFramebufferSize(window, &width, &height);
        glfwWaitEvents(); // While frame buffer size is 0 -> wait()
    }

    vkDeviceWaitIdle(RendererContext::getInstance().pdevice->getLogicalDevice()); // We shouldn’t touch resources that may still be in use

    cleanupSwapChain();

    r_swapchain.initialize(window);
    r_imageviews.initialize(&r_swapchain);
    r_framebuffer.initialize(&r_swapchain, &r_imageviews, &r_renderpass);
}

// GLFW does not know how to properly call a member function with the right this pointer to our instance
static void framebufferResizeCallback(GLFWwindow* window, int width, int height) {
    auto app = reinterpret_cast<Renderer*>(glfwGetWindowUserPointer(window));
    app->framebufferResized = true;
}
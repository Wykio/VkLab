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

    window = glfwCreateWindow(WIDTH, HEIGHT, "Wykio Renderer", nullptr, nullptr);
}

// Initializes Vulkan components needed for the application.
void Renderer::initVulkan() {
    r_instance.initialize();

    if (enableValidationLayers) {
        r_debugMessenger.initialize(r_instance.getInstance());
    }

    createSurface();

    r_device.initialize(r_instance.getInstance(), &surface, &graphicsQueue, &presentQueue);
}

// Runs the main event loop of the application.
void Renderer::mainLoop() {
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
    }
}

// Cleans up all Vulkan and GLFW resources.
void Renderer::cleanup() {
    r_device.cleanup();
    //vkDestroyDevice(device, nullptr);

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
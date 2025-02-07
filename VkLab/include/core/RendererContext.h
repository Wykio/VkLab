#ifndef RENDERER_CONTEXT_H
#define RENDERER_CONTEXT_H

#include "core/Device.h"

#include <vulkan/vulkan.h>

class Device;

class RendererContext {
public:
    // Access to the unique instance
    static RendererContext& getInstance() {
        static RendererContext instance; // Created on first access
        return instance;
    }

    // Prohibit copying and assignment
    RendererContext(const RendererContext&) = delete;
    RendererContext& operator=(const RendererContext&) = delete;

    // Make the destructor virtual if the class is intended to be derived
    ~RendererContext() = default;

    // Shared Vulkan resources
    VkSurfaceKHR surface = VK_NULL_HANDLE; // Vulkan rendering surface
    Device* pdevice = nullptr; // Physical device and logical device used by the application

private:
    // Private constructor
    RendererContext() = default;
};

#endif // RENDERER_CONTEXT_H
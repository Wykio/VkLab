#ifndef INSTANCE_H
#define INSTANCE_H

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

#include "utils/DebugMessenger.h"

#include <GLFW/glfw3.h>
#include <vector>
#include <string>

class VulkanInstance {
public:
    VulkanInstance();
    ~VulkanInstance();

    void initialize();
    void cleanup();
    VkInstance getInstance() const; // read only once created

private:
    bool checkValidationLayerSupport();
    std::vector<const char*> getRequiredExtensions();

    VkInstance instance;
    std::vector<const char*> requiredExtensions;
};

#endif // INSTANCE_H
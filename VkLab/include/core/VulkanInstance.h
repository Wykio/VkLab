#ifndef INSTANCE_H
#define INSTANCE_H

#include "utils/Debug.h"

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
    VkInstance instance;
    std::vector<const char*> requiredExtensions;

    bool checkValidationLayerSupport();
    std::vector<const char*> getRequiredExtensions();
};

#endif // INSTANCE_H
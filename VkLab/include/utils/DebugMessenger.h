#ifndef DEBUG_MESSENGER_H
#define DEBUG_MESSENGER_H

#include <vulkan/vulkan.h>
#include <iostream>
#include <vector>


const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"
};

class DebugMessenger {
public:
    DebugMessenger();

    void initialize(VkInstance instance);
    void cleanup(VkInstance instance);

private:
    VkResult createDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
    void destroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);

    VkDebugUtilsMessengerEXT debugMessenger;
};

void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);

#endif // DEBUG_MESSENGER_H
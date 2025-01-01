#include "core/Device.h"

Device::Device() {
    physicalDevice = VK_NULL_HANDLE;
    device = VK_NULL_HANDLE;
    psurface = VK_NULL_HANDLE;
}

Device::~Device() {
    cleanup();
}

void Device::initialize(VkInstance instance, VkSurfaceKHR* psurface, VkQueue* pgraphicsQueue, VkQueue* ppresentQueue) {
    this->psurface = psurface;
    pickPhysicalDevice(instance);
    createLogicalDevice(pgraphicsQueue, ppresentQueue);
}

void Device::cleanup() {
    if (device != VK_NULL_HANDLE) {
        vkDestroyDevice(device, nullptr);
    }
    device = VK_NULL_HANDLE;
}

// Selects a suitable GPU with Vulkan support.
void Device::pickPhysicalDevice(VkInstance instance) {
    uint32_t physicalDeviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr);

    if (physicalDeviceCount == 0) {
        throw std::runtime_error("failed to find GPUs with Vulkan support!");
    }

    std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
    vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, physicalDevices.data());

    for (const auto& p : physicalDevices) {
        if (isDeviceSuitable(p, psurface)) {
            physicalDevice = p;
            break;
        }
    }

    if (physicalDevice == VK_NULL_HANDLE) {
        throw std::runtime_error("failed to find a suitable GPU!");
    }
}

// Creates a Vulkan logical device for the selected GPU.
void Device::createLogicalDevice(VkQueue* graphicsQueue, VkQueue* presentQueue) {
    // Next, we need to have multiple VkDeviceQueueCreateInfo structs to create a queue from both families.
    // An elegant way to do that is to create a set of all unique queue families that are necessary for the required queues
    QueueFamilyIndices indices = findQueueFamilies(physicalDevice, psurface);

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = {
        indices.graphicsFamily.value(),
        indices.presentFamily.value()
    };

    float queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies) {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures{};

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    // And modify VkDeviceCreateInfo to point to the vector:
    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pQueueCreateInfos = queueCreateInfos.data();

    createInfo.pEnabledFeatures = &deviceFeatures;

    createInfo.enabledExtensionCount = 0;

    if (enableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
    }
    else {
        createInfo.enabledLayerCount = 0;
    }

    if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS) {
        throw std::runtime_error("failed to create logical device!");
    }

    // call to retrieve the queue handle
    vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, graphicsQueue);
    vkGetDeviceQueue(device, indices.presentFamily.value(), 0, presentQueue);
}

// Checks if the given physical device meets the requirements.
bool isDeviceSuitable(VkPhysicalDevice physicalDevice, VkSurfaceKHR* psurface) {
    QueueFamilyIndices indices = findQueueFamilies(physicalDevice, psurface);
    bool extensionsSupported = checkDeviceExtensionSupport(physicalDevice);
    return indices.isComplete() && extensionsSupported;
}

// Finds queue families that support required operations.
QueueFamilyIndices findQueueFamilies(VkPhysicalDevice physicalDevice, VkSurfaceKHR* psurface) {
    QueueFamilyIndices indices;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

    int i = 0;
    for (const auto& queueFamily : queueFamilies) {
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphicsFamily = i;
        }

        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, *psurface, &presentSupport); // Check if this QueueFamily support presentation
        if (presentSupport) {
            indices.presentFamily = i;
        }

        if (indices.isComplete()) {
            break;
        }

        i++;
    }

    return indices;
}

// Checks if the given physical device support 
bool checkDeviceExtensionSupport(VkPhysicalDevice physicalDevice) {
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, availableExtensions.data());

    std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

    for (const auto& extension : availableExtensions) {
        requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
}
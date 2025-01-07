#ifndef SHADER_UTILS_H
#define SHADER_UTILS_H

#include <vulkan/vulkan.h>
#include <vector>
#include <fstream>

// Read a file and store it in a char vector
static std::vector<char> readFile(const std::string& filename) {
    // Start reading from the end to determine the size of the file and allocate the buffer
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error("failed to open file!");
    }

    size_t fileSize = (size_t)file.tellg();
    std::vector<char> buffer(fileSize);

    // Seek back to the beginning and read all at once
    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();

    return buffer;
}

// Create VkShaderModule from a bytecode shader (SPIR-V shader)
static VkShaderModule createShaderModule(const std::vector<char>& code, VkDevice* device) {
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

    VkShaderModule shaderModule;
    if (vkCreateShaderModule(*device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
        throw std::runtime_error("failed to create shader module!");
    }

    return shaderModule;
}

#endif // SHADER_UTILS_H
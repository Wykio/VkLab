#include "graphics/CommandBuffers.h"


void CommandBuffers::initialize(Device* pdevice, CommandPools* pCommandPools) {
    commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = pCommandPools->getDrawCommandPool();
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY; // Can be submitted to a queue for execution, but cannot be called from other command buffers.
    allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();

    if (vkAllocateCommandBuffers(pdevice->getLogicalDevice(), &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate command buffers!");
    }
}

std::vector<VkCommandBuffer> CommandBuffers::getCommandBuffers() {
    return commandBuffers;
}

VkCommandBuffer CommandBuffers::getCommandBuffer(const int index) {
    return commandBuffers[index];
}

VkCommandBuffer* CommandBuffers::getCommandBufferPtr(const int index) {
    return &commandBuffers[index];
}

// We pass the command buffer and the index of the current swapchain image we want to write to
void recordCommandBuffer(
    VkCommandBuffer commandBuffer,
    uint32_t currentFrame,
    uint32_t imageIndex,
    SwapChain* pSwapChain,
    RenderPass* pRenderPass,
    DescriptorSet* pDescriptorSet,
    Pipeline* pPipeline,
    FrameBuffers* pFrameBuffer,
    BufferManager* pBufferManager
) {
    
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("failed to begin recording command buffer!");
    }

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = pRenderPass->getRenderPass();
    renderPassInfo.framebuffer = pFrameBuffer->getSwapChainFramebuffers()[imageIndex];
    renderPassInfo.renderArea.offset = { 0, 0 };
    renderPassInfo.renderArea.extent = pSwapChain->getSwapChainExtent();

    VkClearValue clearColor = { {{0.0f, 0.0f, 0.0f, 1.0f}} };
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pPipeline->getGraphicsPipeline());

    // Bind the vertex buffer
    VkBuffer vertexBuffers[] = { pBufferManager->getVertexBuffer() };
    VkDeviceSize offsets[] = { 0 };
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets); // Bind vertex buffers to bindings

    // Bind the index buffer
    VkBuffer indexBuffers = pBufferManager->getIndexBuffer();
    vkCmdBindIndexBuffer(commandBuffer, indexBuffers, 0, VK_INDEX_TYPE_UINT16);

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(pSwapChain->getSwapChainExtent().width);
    viewport.height = static_cast<float>(pSwapChain->getSwapChainExtent().height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = { 0, 0 };
    scissor.extent = pSwapChain->getSwapChainExtent();
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    // Bind Descriptor Sets
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pPipeline->getPipelineLayout(), 0, 1, pDescriptorSet->getDescriptorSetPtr(currentFrame), 0, nullptr);

    //vkCmdDraw(commandBuffer, static_cast<uint32_t>(vertices.size()), 1, 0, 0); // Without indexes
    vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);

    vkCmdEndRenderPass(commandBuffer);

    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to record command buffer!");
    }
}
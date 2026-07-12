#include "first_app.hpp"

#include <GLFW/glfw3.h>
#include <array>
#include <cstdint>
#include <memory>
#include <stdexcept>
#include <vulkan/vulkan_core.h>

namespace lve {
FirstApp::FirstApp() {
  createPipelinelayout();
  createPipeline();
  createCommandBuffers();
}

FirstApp::~FirstApp() {
  vkDestroyPipelineLayout(lveDevice.device(), pipelineLayout, nullptr);
}

void FirstApp::run() {
  while (!lveWindow.shouldClose()) {
    glfwPollEvents();
    drawFrame();
  }
  vkDeviceWaitIdle(lveDevice.device());
}

void FirstApp::createPipelinelayout() {
  VkPipelineLayoutCreateInfo pipelineLayoutInfo{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
      .setLayoutCount = 0,
      .pSetLayouts = nullptr,
      .pushConstantRangeCount = 0,
      .pPushConstantRanges = nullptr,
  };

  if (vkCreatePipelineLayout(lveDevice.device(), &pipelineLayoutInfo, nullptr,
                             &pipelineLayout) != VK_SUCCESS) {
    throw std::runtime_error("Failed to create Pipeline layout");
  }
}

void FirstApp::createPipeline() {
  auto pipelineConfig = LvePipeline::defaultPipelineConfigInfo(
      lveSwapchain.width(), lveSwapchain.height());
  pipelineConfig.renderPass = lveSwapchain.getRenderPass();
  pipelineConfig.pipelineLayout = pipelineLayout;
  lvePipeline = std::make_unique<LvePipeline>(
      lveDevice, "build/shaders/simple_shader.vert.spv",
      "build/shaders/simple_shader.frag.spv", pipelineConfig);
}

void FirstApp::createCommandBuffers() {
  commandBuffers.resize(lveSwapchain.imageCount());
  VkCommandBufferAllocateInfo allocInfo{
      .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
      .commandPool = lveDevice.getCommandPool(),
      .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
      .commandBufferCount = static_cast<uint32_t>(commandBuffers.size()),
  };

  if (vkAllocateCommandBuffers(lveDevice.device(), &allocInfo,
                               commandBuffers.data()) != VK_SUCCESS) {
    throw std::runtime_error("failed to allocate command buffers");
  }

  for (int i = 0; i < commandBuffers.size(); i++) {
    VkCommandBufferBeginInfo beginInfo{
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
    if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS) {
      throw std::runtime_error("Failed to begin recording command buffer");
    }

    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = {0.1f, 0.1f, 0.1f, 0.1f};
    clearValues[1].depthStencil = {1, 0};

    VkRenderPassBeginInfo renderPassInfo{
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .renderPass = lveSwapchain.getRenderPass(),
        .framebuffer = lveSwapchain.getFrameBuffer(i),
        .renderArea{.offset = {0, 0},
                    .extent = lveSwapchain.getSwapChainExtent()},
        .clearValueCount = static_cast<uint32_t>(clearValues.size()),
        .pClearValues = clearValues.data(),
    };

    vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo,
                         VK_SUBPASS_CONTENTS_INLINE);
    lvePipeline->bind(commandBuffers[i]);
    vkCmdDraw(commandBuffers[i], 3, 1, 0, 0);

    vkCmdEndRenderPass(commandBuffers[i]);
    if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS) {
      throw std::runtime_error("failed to record command buffer");
    }
  }
}

void FirstApp::drawFrame() {
  uint32_t imageIdx;
  auto result = lveSwapchain.acquireNextImage(&imageIdx);

  if(result != VK_SUBOPTIMAL_KHR && result != VK_SUCCESS) {
    throw std::runtime_error("Failed to acquire swapchain image");
  }

  result = lveSwapchain.submitCommandBuffers(&commandBuffers[imageIdx], &imageIdx);
  if(result != VK_SUCCESS) {
    throw std::runtime_error("Failed to present swapchain image");
  }
}
} // namespace lve

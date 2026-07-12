#pragma once

#include "../core/lve_device.hpp"
#include "../core/lve_pipeline.hpp"
#include "../core/lve_swap_chain.hpp"
#include "../core/lve_window.hpp"

// std
#include <memory>
#include <vector>

namespace lve {
class FirstApp {
public:
  static constexpr int WIDTH = 800;
  static constexpr int HEIGHT = 600;

  FirstApp();
  ~FirstApp();

  FirstApp(const FirstApp &) = delete;
  void operator=(const FirstApp &) = delete;

  void run();

private:
  LveWindow lveWindow{WIDTH, HEIGHT, "Hello vulkan!"};
  LveDevice lveDevice{lveWindow};
  LveSwapChain lveSwapchain{lveDevice, lveWindow.getExtent()};
  std::unique_ptr<LvePipeline> lvePipeline;
  VkPipelineLayout pipelineLayout;
  std::vector<VkCommandBuffer> commandBuffers;

  void createPipelinelayout();
  void createPipeline();
  void createCommandBuffers();
  void drawFrame();
};
} // namespace lve

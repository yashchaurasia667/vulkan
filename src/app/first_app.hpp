#pragma once

#include "../core/lve_device.hpp"
#include "../core/lve_pipeline.hpp"
#include "../core/lve_window.hpp"

namespace lve {
class FirstApp {
public:
  static constexpr int WIDTH = 800;
  static constexpr int HEIGHT = 600;

  void run();

private:
  LveWindow lveWindow{WIDTH, HEIGHT, "Hello vulkan!"};
  LveDevice lveDevice{lveWindow};
  LvePipeline lvePipeline{lveDevice, "./build/shaders/simple_shader.vert.spv",
                          "./build/shaders/simple_shader.frag.spv",
                          LvePipeline::defaultPipelineConfigInfo(WIDTH, HEIGHT)};
};
} // namespace lve

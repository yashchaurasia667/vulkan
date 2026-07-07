#pragma once

#include "lve_device.hpp"

#include <cstdint>
#include <string>
#include <vector>

namespace lve {
struct PipelineConfigInfo {};

class LvePipeline {
public:
  LvePipeline(LveDevice &device, const std::string &vertPath,
              const std::string &fragPath, const PipelineConfigInfo configInfo);
  ~LvePipeline() {}

  LvePipeline(const LvePipeline &) = delete;
  void operator=(const LvePipeline &) = delete;

  static PipelineConfigInfo defaultPipelineConfigInfo(uint32_t width, uint32_t height);

private:
  static std::vector<char> readFile(const std::string &filePath);
  void createGraphicsPipeline(const std::string &vertPath,
                              const std::string &fragPath,
                              const PipelineConfigInfo configInfo);

  void createShaderModule(const std::vector<char> shaderSource,
                          VkShaderModule *shaderModule);

  LveDevice &lveDevice;
  VkPipeline graphicsPipeline;
  VkShaderModule vertShaderModule;
  VkShaderModule fragShaderModule;
};
} // namespace lve

#include "lve_pipeline.hpp"
#include "lve_window.hpp"

#include <cstdint>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <vector>

namespace lve {
LvePipeline::LvePipeline(LveDevice &device, const std::string &vertPath,
                         const std::string &fragPath,
                         const PipelineConfigInfo configInfo)
    : lveDevice(device) {
  createGraphicsPipeline(vertPath, fragPath, configInfo);
}

void LvePipeline::createGraphicsPipeline(const std::string &vertPath,
                                         const std::string &fragPath,
                                         const PipelineConfigInfo configInfo) {
  auto vertSrc = readFile(vertPath);
  auto fragSrc = readFile(fragPath);

  std::cout << "vertex shader size: " << vertSrc.size() << std::endl;
  std::cout << "fragment shader size: " << fragSrc.size() << std::endl;
}

std::vector<char> LvePipeline::readFile(const std::string &filePath) {
  std::ifstream file(filePath, std::ios::ate | std::ios::binary);
  if (!file.is_open()) {
    throw std::runtime_error("Failed to open file: " + filePath);
  }

  size_t fileSize = static_cast<size_t>(file.tellg());
  file.seekg(0);

  std::vector<char> buffer(fileSize);
  file.read(buffer.data(), fileSize);

  return buffer;
}

void LvePipeline::createShaderModule(const std::vector<char> shaderSource,
                                     VkShaderModule *shaderModule) {
  VkShaderModuleCreateInfo createInfo{
      .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
      .codeSize = static_cast<uint32_t>(shaderSource.size()),
      .pCode = reinterpret_cast<const uint32_t *>(shaderSource.data()),
  };

  if (vkCreateShaderModule(lveDevice.device(), &createInfo, nullptr,
                           shaderModule) != VK_SUCCESS) {
    throw std::runtime_error("Failed to create logical device");
  }
}
PipelineConfigInfo LvePipeline::defaultPipelineConfigInfo(uint32_t width,
                                                          uint32_t height) {
  PipelineConfigInfo configInfo{};
  return configInfo;
}
} // namespace lve

#include <iostream>
#include <core/vk_engine.h>

int main() {
  std::cout << "vulkan engine" << std::endl;
  VulkanEngine engine;
  engine.init();
  engine.run();
  engine.cleanup();
}

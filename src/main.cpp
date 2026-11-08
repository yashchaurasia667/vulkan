#include <iostream>
#include <core/vk_engine.h>

int main() {
  std::cout << "vulkan engine" << std::endl;
  VulkanEngine engine;

  // initialize vulkan
  engine.init();
  // main loop
  engine.run();
  // free memory
  engine.cleanup();
}

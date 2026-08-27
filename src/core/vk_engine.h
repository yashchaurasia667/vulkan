#pragma once

#include <SDL3/SDL_video.h>
#include <core/vk_types.h>
#include <vulkan/vulkan_core.h>

class VulkanEngine {
public:
  bool _isInitialized{false};
  int _frameNumber{0};
  bool stop_rendering{false};
  VkExtent2D _windowExtent{1700, 900};

  struct SDL_Window *_window{nullptr};
  static VulkanEngine &Get();

  VkInstance _instance;
  VkDebugUtilsMessengerEXT _debug_messenger;
  VkPhysicalDevice _chosenGpu;
  VkDevice _device;
  VkSurfaceKHR _surface;

  void init();
  void run();
  void draw();
  void cleanup();

private:
  void init_vulkan();
  void init_swapchain();
  void init_commands();
  void init_sync_structures();
};

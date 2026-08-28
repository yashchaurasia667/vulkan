#pragma once

#include <core/vk_types.h>
#include <vulkan/vulkan_core.h>

struct FrameData {
  VkCommandPool _commandPool;
  VkCommandBuffer _mainCommandBuffer;
  
  VkSemaphore _swapchainSemaphore, _renderSemaphore;
  VkFence _renderFence;
};

constexpr unsigned int FRAME_OVERLAP = 2;

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

  VkSwapchainKHR _swapchain;
  VkFormat _swapchainImageFormat;

  std::vector<VkImage> _swapchainImages;
  std::vector<VkImageView> _swapchainImageViews;
  VkExtent2D _swapchainExtent;

  FrameData _frames[FRAME_OVERLAP];
  FrameData &get_current_frame() {
    return _frames[_frameNumber % FRAME_OVERLAP];
  }

  VkQueue _graphicsQueue;
  uint32_t _graphicsQueueFamily;

  void init();
  void run();
  void draw();
  void cleanup();

private:
  void init_vulkan();
  void init_swapchain();
  void init_commands();
  void init_sync_structures();

  void create_swapchain(uint32_t width, uint32_t height);
  void destroy_swapchain();
};

#pragma once

#include "core/vk_descriptors.h"
#include <core/vk_types.h>
#include <deque>

struct DeletionQueue {
  std::deque<std::function<void()>> deletors;

  void push_function(std::function<void()> &&function) {
    deletors.push_back(function);
  }
  void flush() {
    for (auto it = deletors.rbegin(); it != deletors.rend(); it++) {
      (*it)();
    }
    deletors.clear();
  }
};

struct FrameData {
  VkCommandPool _commandPool;
  VkCommandBuffer _mainCommandBuffer;

  VkSemaphore _swapchainSemaphore, _renderSemaphore;
  VkFence _renderFence;

  DeletionQueue _deletionQueue;
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

  AllocatedImage _drawImage;
  VkExtent2D _drawExtent;

  DescriptorAllocator globalDescriptorAllocator;
  VkDescriptorSet _drawImageDescriptors;
  VkDescriptorSetLayout _drawImageDescriptorLayout;

  FrameData _frames[FRAME_OVERLAP];
  FrameData &get_current_frame() {
    return _frames[_frameNumber % FRAME_OVERLAP];
  }

  VkQueue _graphicsQueue;
  uint32_t _graphicsQueueFamily;

  VkPipeline _gradientPipeline;
  VkPipelineLayout _gradientPipelineLayout;

  DeletionQueue _mainDeletionQueue;
  VmaAllocator _allocator;

  void init();
  void run();
  void cleanup();

private:
  void draw();
  void draw_background(VkCommandBuffer cmd);

  void init_vulkan();
  void init_swapchain();
  void init_commands();
  void init_sync_structures();

  void init_descriptors();
  void init_pipelines();

  void create_swapchain(uint32_t width, uint32_t height);
  void init_background_pipelines();
  void destroy_swapchain();
};

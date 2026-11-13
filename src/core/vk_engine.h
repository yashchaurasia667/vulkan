#pragma once
#include <vulkan/vulkan_core.h>
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include "core/vk_descriptors.h"
#include <core/vk_loader.h>
#include <core/vk_types.h>
#include <deque>

// push deletion functions in and flush at end
struct DeletionQueue {
  std::deque<std::function<void()>> deletors;

  void push_function(std::function<void()> &&function) {
    /*
     * queue.push_function([&]() {
     *  ...
     * })
     */
    deletors.push_back(function);
  }
  void flush() {
    for (auto it = deletors.rbegin(); it != deletors.rend(); it++) {
      (*it)();
    }
    deletors.clear();
  }
};

// contains all data that a single frame needs
struct FrameData {
  VkSemaphore _swapchainSemaphore, _renderSemaphore;
  VkFence _renderFence;

  VkCommandPool _commandPool;
  VkCommandBuffer _mainCommandBuffer;

  DeletionQueue _deletionQueue;
  DescriptorAllocatorGrowable _frameDescriptors;
};

// very fast small memory that can be pushed to gpu
struct ComputePushConstants {
  glm::vec4 data1;
  glm::vec4 data2;
  glm::vec4 data3;
  glm::vec4 data4;
};

struct GPUSceneData {
  glm::mat4 view;
  glm::mat4 proj;
  glm::mat4 viewproj;
  glm::vec4 ambientColor;
  glm::vec4 sunlightDirection; // w for sun power
  glm::vec4 sunlightColor;
};

// 1 front and 1 back buffer
constexpr unsigned int FRAME_OVERLAP = 2;

class VulkanEngine {
public:
  bool _isInitialized{false};
  bool stop_rendering{false};
  bool resize_requested{false};
  int _frameNumber{0}; // current frame

  // window stuff
  VkExtent2D _windowExtent{1700, 900}; // window size
  struct SDL_Window *_window{nullptr};
  static VulkanEngine &Get();

  // necessary vulkan stuff
  VkInstance _instance; // gives access to most vulkan features
  VkDebugUtilsMessengerEXT _debug_messenger; // for debug callbacks
  VkPhysicalDevice _chosenGpu; // pointer to the chosen physical gpu
  VkDevice _device;            // handle to the chosen gpu
  VkSurfaceKHR
      _surface; // can't draw directly on a window need a surface to draw on

  // swapchain stuff
  VkSwapchainKHR
      _swapchain; // handles frames in flight (front and back buffers)
  VkFormat _swapchainImageFormat;        // image format to use for the images
  std::vector<VkImage> _swapchainImages; // literally the front and back buffers
  std::vector<VkImageView>
      _swapchainImageViews; // handle to the front and back buffers since can't
                            // alter them directly
  VkExtent2D _swapchainExtent; // size of swapchain images

  // intermidiate image stuff
  AllocatedImage _drawImage;  // the intermediate image to draw on then transfer
                              // to swapchain
  AllocatedImage _depthImage; // to draw the depth on
  VkExtent2D _drawExtent;     // size of draw image
  float renderScale = 1.f;

  // descriptors
  DescriptorAllocator
      globalDescriptorAllocator; // memory pool to allocate descriptor sets from
  VkDescriptorSetLayout
      _drawImageDescriptorLayout; // the structure of the descriptor sets(data
                                  // to send to shader in layout)
  VkDescriptorSet _drawImageDescriptors; // an instance of a descritor set
                                         // layout with real bindings

  // texture stuff
  AllocatedImage _whiteImage;
  AllocatedImage _blackImage;
  AllocatedImage _greyImage;
  AllocatedImage _errorCheckerboardImage;

  VkSampler _defaultSamplerLinear;
  VkSampler _defaultSamplerNearest;
  VkDescriptorSetLayout _singleImageDescriptorLayout;

  FrameData _frames[FRAME_OVERLAP];
  GPUSceneData sceneData;
  VkDescriptorSetLayout _gpuSceneDataDescriptorLayout;

  // select a queue to submit tasks to
  VkQueue _graphicsQueue;
  uint32_t _graphicsQueueFamily;

  // compute pipeline
  VkPipeline _gradientPipeline;
  VkPipelineLayout _gradientPipelineLayout;

  // mesh pipeline
  VkPipelineLayout _meshPipelineLayout;
  VkPipeline _meshPipeline;
  GPUMeshBuffers rectangle;
  std::vector<std::shared_ptr<MeshAsset>> testMeshes;

  DeletionQueue _mainDeletionQueue;
  VmaAllocator _allocator;

  // imgui
  VkFence _immFence;
  VkCommandBuffer _immCommandBuffer;
  VkCommandPool _immCommandPool;
  void immediate_submit(std::function<void(VkCommandBuffer cmd)> &&function);

  void init();
  void run();
  void cleanup();
  GPUMeshBuffers uploadMesh(std::span<uint32_t> indices,
                            std::span<Vertex> vertices);

  FrameData &get_current_frame() {
    return _frames[_frameNumber % FRAME_OVERLAP];
  }

private:
  void draw();
  void draw_background(VkCommandBuffer cmd);
  void draw_geometry(VkCommandBuffer cmd);

  void init_vulkan();
  void init_swapchain();
  void resize_swapchain();
  void init_commands();
  void init_sync_structures();

  void init_descriptors();
  void init_pipelines();
  void init_mesh_pipeline();
  void init_default_data();

  void create_swapchain(uint32_t width, uint32_t height);
  void init_background_pipelines();
  void init_imgui();
  void draw_imgui(VkCommandBuffer cmd, VkImageView targetImageView);

  void destroy_swapchain();
  AllocatedBuffer create_buffer(size_t allocSize, VkBufferUsageFlags usage,
                                VmaMemoryUsage memoryUsage);
  void destroy_buffer(const AllocatedBuffer &buffer);

  AllocatedImage create_image(VkExtent3D size, VkFormat format,
                              VkImageUsageFlags usage, bool mipmapped = false);
  AllocatedImage create_image(void *data, VkExtent3D size, VkFormat format,
                              VkImageUsageFlags usage, bool mipmapped = false);
  void destroy_image(const AllocatedImage &img);
};

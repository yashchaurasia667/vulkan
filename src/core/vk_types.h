#pragma once

// #include <array>
// #include <deque>
// #include <functional>
// #include <memory>
// #include <optional>
// #include <span>
// #include <string>
// #include <vector>

#include <vma/vk_mem_alloc.h>
#include <vulkan/vk_enum_string_helper.h>
#include <vulkan/vulkan.h>

#include <fmt/core.h>

#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>

#define VK_CHECK(x)                                                            \
  do {                                                                         \
    VkResult err = x;                                                          \
    if (err) {                                                                 \
      fmt::print("Detected Vulkan Error: {}", string_VkResult(err));           \
      abort();                                                                 \
    }                                                                          \
  } while(0);

struct AllocatedImage {
  VkImage image;
  VkImageView imageView;
  VmaAllocation allocation;
  VkExtent3D imageExtent;
  VkFormat imageFormat;
};

struct AllocatedBuffer {
  VkBuffer buffer;
  VmaAllocation allocation;
  VmaAllocationInfo info;
};

struct Vertex {
  glm::vec3 position;
  float uv_x;
  glm::vec3 normal;
  float uv_y;
  glm::vec4 color;
};

struct GPUMeshBuffers {
  AllocatedBuffer indexBuffer;
  AllocatedBuffer vertexBuffer;
  VkDeviceAddress vertexBufferAddress;
};

struct GPUDrawPushConstants {
  glm::mat4 worldMatrix;
  VkDeviceAddress vertexBuffer;
};

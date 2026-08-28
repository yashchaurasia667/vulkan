#pragma once

#include <vulkan/vulkan.h>

namespace vkutil {
void transition_image(VkCommandBuffer cmd, VkImage img,
                      VkImageLayout currentLayout, VkImageLayout newLayout);
}

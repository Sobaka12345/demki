#pragma once

#include <glm/vec3.hpp>
#include <vulkan/vulkan_core.h>

#include <array>

namespace vk {

struct Vertex3DColored
{
    glm::vec3 pos;
    glm::vec3 color;

    static constexpr std::array<VkVertexInputBindingDescription, 1> getBindingDescription() {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex3DColored);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return { bindingDescription };
    }

    static constexpr std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions() {
        std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};
        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex3DColored, pos);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex3DColored, color);

        return attributeDescriptions;
    }
};

}

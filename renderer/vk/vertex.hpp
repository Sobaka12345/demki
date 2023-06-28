#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <vulkan/vulkan_core.h>

#include <array>

namespace vk {

struct Vertex3DColored
{
    glm::vec3 pos;
    glm::vec3 color;

    static constexpr std::array<VkVertexInputBindingDescription, 1> bindingDescription() {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex3DColored);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return { bindingDescription };
    }

    static constexpr std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions() {
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

struct Vertex3DColoredTextured
{
    glm::vec3 pos;
    glm::vec3 color;
    glm::vec2 texture;


    bool operator==(const vk::Vertex3DColoredTextured& other) const {
        return pos == other.pos && color == other.color && texture == other.texture;
    }

    static constexpr std::array<VkVertexInputBindingDescription, 1> bindingDescription() {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex3DColoredTextured);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return { bindingDescription };
    }

    static constexpr std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions() {
        std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};
        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex3DColoredTextured, pos);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex3DColoredTextured, color);

        attributeDescriptions[2].binding = 0;
        attributeDescriptions[2].location = 2;
        attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[2].offset = offsetof(Vertex3DColoredTextured, texture);

        return attributeDescriptions;
    }
};

}

namespace std {

template<> struct hash<vk::Vertex3DColoredTextured> {
    size_t operator()(vk::Vertex3DColoredTextured const& vertex) const {
        return 
            ((hash<glm::vec3>()(vertex.pos) ^ (hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^
            (hash<glm::vec2>()(vertex.texture) << 1);
    }
};

}
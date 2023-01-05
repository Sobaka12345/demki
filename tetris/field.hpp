#pragma once

#include "../renderer/uniform_buffer.hpp"
#include "../renderer/descriptor_set.hpp"

#include "figure.hpp"

#include <cstdint>

class Field : public Renderable
{
public:
    static constexpr uint32_t s_width = 14;
    static constexpr uint32_t s_height = 24;
    static constexpr uint32_t s_middleX = s_width / 2 - 1;
    static constexpr uint32_t s_middleY = s_height / 2 - 1;
    static constexpr uint32_t s_objectsCount = s_width * s_height;

    static bool isCollided(Position position);

public:
    Field(const vk::DescriptorSet* descriptorSet, vk::IUBOProvider* uboProvider);
    virtual void draw(VkCommandBuffer commandBuffer) const override;
    virtual void setModel(std::weak_ptr<Model> model) override;

private:
    std::vector<Block> m_blocks;
};

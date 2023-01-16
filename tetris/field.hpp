#pragma once

#include "position.hpp"

#include <renderable.hpp>
#include <uniform_buffer.hpp>
#include <descriptor_set.hpp>

#include <cstdint>

class Block;
class Figure;

class Field : public Renderable
{
public:
    static constexpr uint32_t s_width = 14;
    static constexpr uint32_t s_height = 24;
    static constexpr uint32_t s_borders = s_width * 2 + s_height * 2 - 4;
    static constexpr uint32_t s_middleX = s_width / 2;
    static constexpr uint32_t s_middleY = s_height / 2;
    static constexpr uint32_t s_objectsCount = s_width * s_height;

    using RowType = std::array<std::shared_ptr<Block>, s_width>;
    using FieldType = std::array<RowType, s_height>;

public:
    Field(vk::DescriptorSet* descriptorSet, vk::IUBOProvider* uboProvider);
    int flushRowsAndSpawnFigure();
    void tryRotateFigure();
    bool tryMoveFigure(int32_t dx, int32_t dy);
    bool isPositionOccupied(Position pos) const;

    bool isBlocksOverflow() const;

    virtual void setModel(std::weak_ptr<Model> model) override;
    virtual void draw(VkCommandBuffer commandBuffer) const override;

private:
    int flushRows(int32_t topRowBound, int32_t bottomRowBound);

private:
    vk::IUBOProvider* m_uboProvider;
    vk::DescriptorSet* m_descriptorSet;
    std::shared_ptr<Figure> m_figure;
    FieldType m_blocks;
};

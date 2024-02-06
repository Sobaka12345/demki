#pragma once

#include "position.hpp"

#include <array>
#include <cstdint>
#include <memory>

class Block;
class Figure;
class OperationContext;

class IModel;
class ITexture;
class IGraphicsContext;

class Field
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
    Field(IGraphicsContext& resources);
    std::shared_ptr<Block> createBlock() const;

    int32_t flushRowsAndSpawnFigure();
    void tryRotateFigure();
    bool tryMoveFigure(int32_t dx, int32_t dy);
    bool isPositionOccupied(Position pos) const;

    bool isBlocksOverflow() const;

    void draw(OperationContext& context) const;

private:
    int flushRows(int32_t topRowBound, int32_t bottomRowBound);

private:
    IGraphicsContext& m_context;

    FieldType m_blocks;
    std::shared_ptr<IModel> m_cube;
    std::shared_ptr<ITexture> m_cubeTexture;

    std::shared_ptr<Figure> m_figure;
};

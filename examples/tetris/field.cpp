#include "field.hpp"

#include "figure.hpp"

#include <renderable.hpp>
#include <igraphics_context.hpp>

using namespace renderer;

static constexpr std::array<Vertex3DColoredTextured, 8> s_cubeVertices = {
    Vertex3DColoredTextured{ { -0.5f, -0.5f, 0.5f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f } },
    Vertex3DColoredTextured{ { 0.5f, -0.5f, 0.5f }, { 0.0f, 1.0f, 0.0f }, { 1.0f, 0.0f } },
    Vertex3DColoredTextured{ { -0.5f, 0.5f, 0.5f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f } },
    Vertex3DColoredTextured{ { 0.5f, 0.5f, 0.5f }, { 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f } },
    Vertex3DColoredTextured{ { -0.5f, -0.5f, -0.5f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f } },
    Vertex3DColoredTextured{ { 0.5f, -0.5f, -0.5f }, { 1.0f, 1.0f, 1.0f }, { 1.0f, 0.0f } },
    Vertex3DColoredTextured{ { -0.5f, 0.5f, -0.5f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 1.0f } },
    Vertex3DColoredTextured{ { 0.5f, 0.5f, -0.5f }, { 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f } },
};

static constexpr std::array<uint32_t, 36> s_cubeIndices = { 7, 6, 2, 2, 3, 7, 0, 4, 5, 5, 1, 0, 0,
    2, 6, 6, 4, 0, 7, 3, 1, 1, 5, 7, 3, 2, 0, 0, 1, 3, 4, 6, 7, 7, 5, 4 };

Field::Field(IGraphicsContext& context)
    : m_context(context)
{
    m_cube = context.createModel(IModel::CreateInfo{
        s_cubeVertices,
        s_cubeIndices,
    });
    m_cubeTexture = context.createTexture(executablePath() / "textures" / "roshi.jpg");

    for (int32_t row = 0; row < m_blocks.size(); ++row)
    {
        for (int32_t col = 0; col < m_blocks[row].size(); ++col)
        {
            if (row != 0 && row != m_blocks.size() - 1 && col != 0 &&
                col != m_blocks[row].size() - 1)
            {
                continue;
            }

            m_blocks[row][col] = createBlock();
            m_blocks[row][col]->setPosition({ col, row });
        }
    }
}

std::shared_ptr<Block> Field::createBlock() const
{
    Renderable obj(m_context);
    obj.setModel(m_cube);
    obj.setTexture(m_cubeTexture);

    return std::make_shared<Block>(std::move(obj));
}

bool Field::tryMoveFigure(int32_t dx, int32_t dy)
{
    return m_figure->tryMove(-dx, dy);
}

bool Field::isPositionOccupied(Position pos) const
{
    return bool(m_blocks[pos.y][pos.x]);
}

bool Field::isBlocksOverflow() const
{
    for (auto& block : m_figure->blocks())
    {
        if (block->position().y <= 0)
        {
            return true;
        }
    }
    return false;
}

int32_t Field::flushRowsAndSpawnFigure()
{
    int32_t flushed = 0;
    if (m_figure)
    {
        int32_t bottomRow = 0, topRow = s_height;
        for (const auto& block : m_figure->blocks())
        {
            const auto pos = block->position();
            if (pos.y > bottomRow)
            {
                bottomRow = pos.y;
            }
            if (pos.y < topRow)
            {
                topRow = pos.y;
            }
            m_blocks[pos.y][pos.x] = block;
        }
        flushed = flushRows(topRow, bottomRow);
    }
    m_figure = FiguresMaker::createRandomFigure(*this);

    return flushed;
}

int Field::flushRows(int32_t topRowBound, int32_t bottomRowBound)
{
    topRowBound = (std::max)(topRowBound, 1);

    int32_t flushedCount = 0;
    for (; topRowBound <= bottomRowBound; ++topRowBound)
    {
        bool flush = true;
        for (auto& block : m_blocks[topRowBound])
        {
            if (!block)
            {
                flush = false;
                break;
            }
        }

        if (!flush) continue;

        ++flushedCount;

        for (int32_t col = 1; col < m_blocks[topRowBound].size() - 1; ++col)
        {
            m_blocks[topRowBound][col].reset();
        }

        for (int32_t row = topRowBound - 1; row > 1; --row)
        {
            for (int32_t col = 1; col < m_blocks[row].size() - 1; ++col)
            {
                auto& block = m_blocks[row][col];
                if (block)
                {
                    std::swap(block, m_blocks[row + 1][col]);
                    m_blocks[row + 1][col]->move(0, 1);
                }
            }
        }
    }

    return flushedCount;
}

void Field::tryRotateFigure()
{
    if (m_figure)
    {
        m_figure->tryRotate();
    }
}

void Field::draw(renderer::OperationContext& context) const
{
    if (m_figure)
    {
        m_figure->draw(context);
    }

    for (auto& blockLine : m_blocks)
    {
        for (auto& block : blockLine)
        {
            if (block)
            {
                block->draw(context);
            }
        }
    }
}

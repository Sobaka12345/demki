#include "field.hpp"

#include "figure.hpp"

Field::Field(vk::DescriptorSet* descriptorSet, vk::IUBOProvider* uboProvider)
{
    m_uboProvider = uboProvider;
    m_descriptorSet = descriptorSet;

    for (int32_t row = 0; row < m_blocks.size(); ++row)
    {
        for (int32_t col = 0; col < m_blocks[row].size(); ++col)
        {
            if (row != 0 && row != m_blocks.size() - 1 &&
                col != 0 && col != m_blocks[row].size() - 1)
            {
                continue;
            }
            m_blocks[row][col] = std::make_shared<Block>(
                std::make_unique<vk::UBOValue<vk::UBOModel>>(descriptorSet, uboProvider->tryGetUBOHandler()));
            m_blocks[row][col]->setPosition({col, row});
        }
    }
}

bool Field::tryMoveFigure(int32_t dx, int32_t dy)
{
    return m_figure->tryMove(dx, dy);
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
            if (pos.y > bottomRow) { bottomRow = pos.y; }
            if (pos.y < topRow) { topRow = pos.y; }
            m_blocks[pos.y][pos.x] = block;
        }
        flushed = flushRows(topRow, bottomRow);
    }
    m_figure = FiguresMaker::createRandomFigure(this, m_descriptorSet, m_uboProvider);
    m_figure->setModel(model());

    return flushed;
}

int Field::flushRows(int32_t topRowBound, int32_t bottomRowBound)
{
    topRowBound = std::max(topRowBound, 1);

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

void Field::draw(VkCommandBuffer commandBuffer) const
{
    if (m_figure)
    {
        m_figure->draw(commandBuffer);
    }

    for (auto& blockLine : m_blocks)
    {
        for (auto& block : blockLine)
        {
            if (block)
            {
                block->draw(commandBuffer);
            }
        }
    }
}

//void Field::rotateRows(std::array::reverse_iterator lhs, std::array::reverse_iterator newBegin, std::array::reverse_iterator rhs)
//{

//}

//void Field::swapRows(size_t lRow, size_t rRow)
//{
//    std::rotate()
//}

void Field::setModel(std::weak_ptr<Model> model)
{
    for (auto& blockLine : m_blocks)
    {
        for (auto& block : blockLine)
        {
            if (block)
            {
                block->setModel(model);
            }
        }
    }
    if (m_figure)
    {
        m_figure->setModel(model);
    }

    Renderable::setModel(model);
}

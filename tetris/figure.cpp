#include "figure.hpp"

#include "field.hpp"

Block::Block(std::unique_ptr<vk::UBOValue<vk::UBOModel>> position3D)
    : m_position3D(std::move(position3D))
{
}

void Block::draw(const vk::CommandBuffer& commandBuffer) const
{
    m_position3D->bind(commandBuffer);
    Renderable::draw(commandBuffer);
}

void Block::move(int32_t dx, int32_t dy)
{
    return setPosition(Position{
        m_position.x + dx,
        m_position.y + dy
    });
}

void Block::setPosition(Position position)
{
    m_position = position;
    m_position3D->set(vk::UBOModel{position.position3D()});
    m_position3D->sync();
}

Position Block::position() const
{
    return m_position;
}

Figure::Figure(const Field* field,
    const vk::DescriptorSet* descriptorSet, vk::IUBOProvider* uboProvider)
    : m_field(field)
{
    for (auto& block : m_blocks)
    {
        block = std::make_shared<Block>(std::make_unique<vk::UBOValue<vk::UBOModel>>(
            descriptorSet, uboProvider->tryGetUBOHandler()));
    }
}

const std::array<std::shared_ptr<Block>, Figure::s_blocksCount>& Figure::blocks() const
{
    return m_blocks;
}

bool Figure::hitTest(Position pos) const
{
    for (auto& block : m_blocks)
    {
        if (block->position() == pos)
        {
            return true;
        }
    }

    return false;
}

bool Figure::tryMove(int32_t dx, int32_t dy)
{
    for (auto& block : m_blocks)
    {
        const Position newPosition {
            block->position().x + dx,
            block->position().y + dy
        };
        if (m_field->isPositionOccupied(newPosition))
        {
            return false;
        }
    }

    for (auto& block : m_blocks) { block->move(dx, dy); }

    return true;
}

bool Figure::tryRotate()
{
    const Position anchor = rotationAnchor();
    std::vector<Position> newPositions(m_blocks.size());
    for (size_t i = 0; i < m_blocks.size(); ++i)
    {
        newPositions[i].x = -(m_blocks[i]->position().y - anchor.y) + anchor.x;
        newPositions[i].y = (m_blocks[i]->position().x - anchor.x) + anchor.y;
        if (m_field->isPositionOccupied(newPositions[i]))
        {
            return false;
        }
    }

    for (size_t i = 0; i < m_blocks.size(); ++i) { m_blocks[i]->setPosition(newPositions[i]); }

    return true;
}

void Figure::draw(const vk::CommandBuffer& commandBuffer) const
{
    for (auto& block : m_blocks)
    {
        block->draw(commandBuffer);
    }
}

void Figure::setModel(std::weak_ptr<Model> model)
{
    for(auto& block : m_blocks)
    {
        block->setModel(model);
    }
    Renderable::setModel(model);
}

LFigure::LFigure(const Field* field, const vk::DescriptorSet* descriptorSet, vk::IUBOProvider* uboProvider)
    : Figure(field, descriptorSet, uboProvider)
{
    m_blocks[0]->setPosition({Field::s_middleX    , 0});
    m_blocks[1]->setPosition({Field::s_middleX    , 1});
    m_blocks[2]->setPosition({Field::s_middleX    , 2});
    m_blocks[3]->setPosition({Field::s_middleX + 1, 2});
}

Position LFigure::rotationAnchor() const
{
    return m_blocks[1]->position();
}

LRFigure::LRFigure(const Field* field, const vk::DescriptorSet* descriptorSet, vk::IUBOProvider* uboProvider)
    : Figure(field, descriptorSet, uboProvider)
{
    m_blocks[0]->setPosition({Field::s_middleX    , 0});
    m_blocks[1]->setPosition({Field::s_middleX    , 1});
    m_blocks[2]->setPosition({Field::s_middleX    , 2});
    m_blocks[3]->setPosition({Field::s_middleX - 1, 2});
}

Position LRFigure::rotationAnchor() const
{
    return m_blocks[1]->position();
}

ZFigure::ZFigure(const Field* field, const vk::DescriptorSet* descriptorSet, vk::IUBOProvider* uboProvider)
    : Figure(field, descriptorSet, uboProvider)
{
    m_blocks[0]->setPosition({Field::s_middleX - 1, 0});
    m_blocks[1]->setPosition({Field::s_middleX    , 0});
    m_blocks[2]->setPosition({Field::s_middleX    , 1});
    m_blocks[3]->setPosition({Field::s_middleX + 1, 1});
}

Position ZFigure::rotationAnchor() const
{
    return m_blocks[1]->position();
}

ZRFigure::ZRFigure(const Field* field, const vk::DescriptorSet* descriptorSet, vk::IUBOProvider* uboProvider)
    : Figure(field, descriptorSet, uboProvider)
{
    m_blocks[0]->setPosition({Field::s_middleX + 1, 0});
    m_blocks[1]->setPosition({Field::s_middleX    , 0});
    m_blocks[2]->setPosition({Field::s_middleX    , 1});
    m_blocks[3]->setPosition({Field::s_middleX - 1, 1});
}

Position ZRFigure::rotationAnchor() const
{
    return m_blocks[1]->position();
}

OFigure::OFigure(const Field* field, const vk::DescriptorSet* descriptorSet, vk::IUBOProvider* uboProvider)
    : Figure(field, descriptorSet, uboProvider)
{
    m_blocks[0]->setPosition({Field::s_middleX    , 0});
    m_blocks[1]->setPosition({Field::s_middleX    , 1});
    m_blocks[2]->setPosition({Field::s_middleX - 1, 0});
    m_blocks[3]->setPosition({Field::s_middleX - 1, 1});
}

bool OFigure::tryRotate()
{
    return false;
}

Position OFigure::rotationAnchor() const
{
    return {};
}

IFigure::IFigure(const Field* field, const vk::DescriptorSet* descriptorSet, vk::IUBOProvider* uboProvider)
    : Figure(field, descriptorSet, uboProvider)
{
    m_blocks[0]->setPosition({Field::s_middleX, 0});
    m_blocks[1]->setPosition({Field::s_middleX, 1});
    m_blocks[2]->setPosition({Field::s_middleX, 2});
    m_blocks[3]->setPosition({Field::s_middleX, 3});
}

Position IFigure::rotationAnchor() const
{
    return m_blocks[1]->position();
}

TFigure::TFigure(const Field *field, const vk::DescriptorSet *descriptorSet, vk::IUBOProvider *uboProvider)
    : Figure(field, descriptorSet, uboProvider)
{
    m_blocks[0]->setPosition({Field::s_middleX    , 0});
    m_blocks[1]->setPosition({Field::s_middleX - 1, 1});
    m_blocks[2]->setPosition({Field::s_middleX    , 1});
    m_blocks[3]->setPosition({Field::s_middleX    , 2});
}

Position TFigure::rotationAnchor() const
{
    return m_blocks[2]->position();
}




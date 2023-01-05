#include "figure.hpp"

#include "field.hpp"

Block::Block(std::unique_ptr<vk::UBOValue<vk::UBOModel>> position3D)
    : m_position3D(std::move(position3D))
{
}

void Block::draw(VkCommandBuffer commandBuffer) const
{
    m_position3D->bind(commandBuffer);
    Renderable::draw(commandBuffer);
}

void Block::move(int32_t dx, int32_t dy)
{
    setPosition(Position{
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

Figure::Figure(const vk::DescriptorSet* descriptorSet, vk::IUBOProvider* uboProvider)
    : m_descriptorSet(descriptorSet)
{
    m_blocks.reserve(s_blocksCount);
    for (size_t i = 0; i < s_blocksCount; ++i)
    {
        m_blocks.emplace_back(std::make_unique<vk::UBOValue<vk::UBOModel>>(
            m_descriptorSet, uboProvider->tryGetUBOHandler())
        );
    }
}

void Figure::move(int32_t dx, int32_t dy)
{
    for(auto& block : m_blocks)
    {
        block.move(dx, dy);
    }
}

void Figure::draw(VkCommandBuffer commandBuffer) const
{
    for(auto& block : m_blocks)
    {
        block.draw(commandBuffer);
    }
}

void Figure::setModel(std::weak_ptr<Model> model)
{
    for(auto& block : m_blocks)
    {
        block.setModel(model);
    }
    Renderable::setModel(model);
}

LFigure::LFigure(const vk::DescriptorSet* descriptorSet, vk::IUBOProvider* uboProvider)
    : Figure(descriptorSet, uboProvider)
{
    m_blocks[0].setPosition({Field::s_middleX    , 0});
    m_blocks[1].setPosition({Field::s_middleX    , 1});
    m_blocks[2].setPosition({Field::s_middleX    , 2});
    m_blocks[3].setPosition({Field::s_middleX + 1, 2});
}

LRFigure::LRFigure(const vk::DescriptorSet* descriptorSet, vk::IUBOProvider* uboProvider)
    : Figure(descriptorSet, uboProvider)
{
    m_blocks[0].setPosition({Field::s_middleX    , 0});
    m_blocks[1].setPosition({Field::s_middleX    , 1});
    m_blocks[2].setPosition({Field::s_middleX    , 2});
    m_blocks[3].setPosition({Field::s_middleX - 1, 2});
}

ZFigure::ZFigure(const vk::DescriptorSet* descriptorSet, vk::IUBOProvider* uboProvider)
    : Figure(descriptorSet, uboProvider)
{
    m_blocks[0].setPosition({Field::s_middleX - 1, 0});
    m_blocks[1].setPosition({Field::s_middleX    , 0});
    m_blocks[2].setPosition({Field::s_middleX    , 1});
    m_blocks[3].setPosition({Field::s_middleX + 1, 1});
}

ZRFigure::ZRFigure(const vk::DescriptorSet* descriptorSet, vk::IUBOProvider* uboProvider)
    : Figure(descriptorSet, uboProvider)
{
    m_blocks[0].setPosition({Field::s_middleX + 1, 0});
    m_blocks[1].setPosition({Field::s_middleX    , 0});
    m_blocks[2].setPosition({Field::s_middleX    , 1});
    m_blocks[3].setPosition({Field::s_middleX - 1, 1});
}

OFigure::OFigure(const vk::DescriptorSet* descriptorSet, vk::IUBOProvider* uboProvider)
    : Figure(descriptorSet, uboProvider)
{
    m_blocks[0].setPosition({Field::s_middleX    , 0});
    m_blocks[1].setPosition({Field::s_middleX    , 1});
    m_blocks[2].setPosition({Field::s_middleX - 1, 0});
    m_blocks[3].setPosition({Field::s_middleX - 1, 1});
}

IFigure::IFigure(const vk::DescriptorSet* descriptorSet, vk::IUBOProvider* uboProvider)
    : Figure(descriptorSet, uboProvider)
{
    m_blocks[0].setPosition({Field::s_middleX, 0});
    m_blocks[1].setPosition({Field::s_middleX, 1});
    m_blocks[2].setPosition({Field::s_middleX, 2});
    m_blocks[3].setPosition({Field::s_middleX, 3});
}

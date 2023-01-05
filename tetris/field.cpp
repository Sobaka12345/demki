#include "field.hpp"

bool Field::isCollided(Position position)
{
    return
        position.x == 0 ||
        position.x >= Field::s_width - 1 ||
        position.y == Field::s_height - 1;
}

Field::Field(const vk::DescriptorSet *descriptorSet, vk::IUBOProvider *uboProvider)
{
    m_blocks.reserve(s_width * s_height - 4);
    for (int32_t i = 0; i < s_height; ++i)
    {
        for (int32_t j = 0; j < s_width; ++j)
        {
            if (i != 0 && i != s_height - 1 && j != 0 && j != s_width - 1)
            {
                continue;
            }
            auto& ref = m_blocks.emplace_back(
                std::make_unique<vk::UBOValue<vk::UBOModel>>(descriptorSet, uboProvider->tryGetUBOHandler()));
            ref.setPosition({j, i});
        }
    }
}

void Field::draw(VkCommandBuffer commandBuffer) const
{
    for (auto& block : m_blocks)
    {
        block.draw(commandBuffer);
    }
}

void Field::setModel(std::weak_ptr<Model> model)
{
    for (auto& block : m_blocks)
    {
        block.setModel(model);
    }
    Renderable::setModel(model);
}

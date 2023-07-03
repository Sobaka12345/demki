#include "renderable.hpp"

#include "model.hpp"

Renderable::Renderable() {}

Renderable::~Renderable() {}

void Renderable::draw(const vk::handles::CommandBuffer& commandBuffer) const
{
    if (!m_model.expired())
    {
        m_model.lock()->draw(commandBuffer);
    }
}

std::weak_ptr<Model> Renderable::model() const
{
    return m_model;
}

void Renderable::setModel(std::weak_ptr<Model> model)
{
    m_model = model;
}

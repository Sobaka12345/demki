#pragma once

#include "model.hpp"

class Renderable
{
public:
    Renderable();
    virtual ~Renderable();
    virtual void draw(VkCommandBuffer commandBuffer) const;

    std::weak_ptr<Model> model() const;
    virtual void setModel(std::weak_ptr<Model> model);

private:
    std::weak_ptr<Model> m_model;
};

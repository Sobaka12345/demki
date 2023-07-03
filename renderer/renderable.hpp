#pragma once

#include "irenderable.hpp"

class Renderable : public IRenderable
{
public:
    struct Uniforms
    {};

public:
    Renderable();
    virtual ~Renderable();
    virtual void draw(const vk::handles::CommandBuffer& commandBuffer) const override;

    virtual std::weak_ptr<Model> model() const override;
    virtual void setModel(std::weak_ptr<Model> model) override;

private:
    std::weak_ptr<Model> m_model;
};

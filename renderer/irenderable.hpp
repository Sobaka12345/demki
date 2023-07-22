#pragma once

#include <memory>

class IModel;
class ITexture;
class RenderContext;

class IRenderable
{
public:
    virtual ~IRenderable(){};
    virtual void draw(const RenderContext& context) const = 0;
    virtual void bind(::RenderContext& context) = 0;

    virtual std::weak_ptr<IModel> model() const = 0;
    virtual void setModel(std::weak_ptr<IModel> model) = 0;

    virtual std::weak_ptr<ITexture> texture() const = 0;
    virtual void setTexture(std::weak_ptr<ITexture> texture) = 0;
};

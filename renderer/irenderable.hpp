#pragma once

#include <memory>

class IModel;
class ITexture;
class OperationContext;

class IRenderable
{
public:
    virtual ~IRenderable(){};
    virtual void draw(const OperationContext& context) const = 0;
    virtual void bind(::OperationContext& context) = 0;

    virtual std::weak_ptr<IModel> model() const = 0;
    virtual void setModel(std::weak_ptr<IModel> model) = 0;

    virtual std::weak_ptr<ITexture> texture() const = 0;
    virtual void setTexture(std::weak_ptr<ITexture> texture) = 0;
};

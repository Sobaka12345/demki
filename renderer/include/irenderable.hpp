#pragma once

#include <memory>

namespace renderer {

class IMesh;
class ITexture;
class OperationContext;

class IRenderable
{
public:
    virtual ~IRenderable() {};
    virtual void draw(OperationContext& context) const = 0;
    virtual void bind(OperationContext& context) = 0;

    virtual std::weak_ptr<IMesh> mesh() const = 0;
    virtual void setMesh(std::weak_ptr<IMesh> mesh) = 0;

    virtual std::weak_ptr<ITexture> texture() const = 0;
    virtual void setTexture(std::weak_ptr<ITexture> texture) = 0;
};

}    //  namespace renderer

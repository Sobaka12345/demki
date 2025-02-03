#pragma once

#include "uniform_value.hpp"

#include <irenderable.hpp>

#include <ishader_interface_container.hpp>

namespace renderer {

class IGraphicsContext;

class Renderable
    : public IRenderable
{
public:
    Renderable(IGraphicsContext& context);
    virtual ~Renderable();

    virtual void draw(OperationContext& context) const override;

    virtual void bind(OperationContext& context) override;

    virtual std::weak_ptr<IMesh> mesh() const override;
    virtual void setMesh(std::weak_ptr<IMesh> mesh) override;

    virtual std::weak_ptr<ITexture> texture() const override;
    virtual void setTexture(std::weak_ptr<ITexture> value) override;

    void setPosition(glm::mat4 position);
    glm::mat4 position() const;

private:
    std::weak_ptr<IMesh> m_mesh;
    std::weak_ptr<ITexture> m_texture;
};

}    //  namespace renderer

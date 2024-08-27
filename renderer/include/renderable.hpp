#pragma once

#include "uniform_value.hpp"

#include <irenderable.hpp>

#include <ishader_interface.hpp>

#include <glm/mat4x4.hpp>

namespace renderer {

class IGraphicsContext;

class IShaderResourceProvider;

class Renderable
    : public ShaderInterfaceContainer<IShaderInterfaceContainer,
          ShaderInterfaceBindingMeta<glm::mat4,
              ShaderBlockType::UNIFORM_DYNAMIC,
              ShaderStage::VERTEX>,
          ShaderInterfaceBindingMeta<std::nullptr_t,
              ShaderBlockType::SAMPLER,
              ShaderStage::FRAGMENT>>
    , public IRenderable
{
public:
    Renderable(IGraphicsContext& context);
    virtual ~Renderable();

    virtual void draw(OperationContext& context) const override;

    virtual void bind(OperationContext& context) override;
    virtual std::span<const InterfaceDescriptor> uniforms() const override;
    virtual std::span<const InterfaceDescriptor> dynamicUniforms() const override;

    virtual std::weak_ptr<IMesh> mesh() const override;
    virtual void setMesh(std::weak_ptr<IMesh> mesh) override;

    virtual std::weak_ptr<ITexture> texture() const override;
    virtual void setTexture(std::weak_ptr<ITexture> texture) override;

    void setPosition(glm::mat4 position);
    glm::mat4 position() const;

private:
    UniformValue<glm::mat4> m_position;

    std::weak_ptr<IMesh> m_mesh;
    std::weak_ptr<ITexture> m_texture;
};

}    //  namespace renderer

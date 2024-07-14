#pragma once

#include "../uniform_value.hpp"

#include <irenderable.hpp>

#include <ishader_interface.hpp>

#include <glm/mat4x4.hpp>

#include <array>

namespace renderer {

class IShaderResourceProvider;

class Renderable
    : public IRenderable
    , public SIShaderInterfaceContainer<Renderable>
{
public:
    static constexpr ShaderInterfaceLayout<2> s_layout = {
        ShaderInterfaceBinding{
            .type = ShaderBlockType::UNIFORM_DYNAMIC,
            .stage = ShaderStage::VERTEX,
        },
        ShaderInterfaceBinding{
            .type = ShaderBlockType::SAMPLER,
            .stage = ShaderStage::FRAGMENT,
        },
    };

public:
    Renderable(IShaderResourceProvider& provider);
    Renderable(const Renderable& other) noexcept;
    Renderable(Renderable&& other) noexcept;
    virtual ~Renderable();
    virtual void draw(OperationContext& context) const override;

    virtual void bind(OperationContext& context) override;
    virtual std::span<const InterfaceDescriptor> uniforms() const override;
    virtual std::span<const InterfaceDescriptor> dynamicUniforms() const override;

    virtual std::weak_ptr<IModel> model() const override;
    virtual void setModel(std::weak_ptr<IModel> model) override;

    virtual std::weak_ptr<ITexture> texture() const override;
    virtual void setTexture(std::weak_ptr<ITexture> texture) override;

    void setPosition(glm::mat4 position) { m_position.set(std::move(position)); }

    glm::mat4 position() const { return m_position.get(); }

private:
    IShaderResourceProvider& m_shaderResourceProvider;

    std::weak_ptr<IModel> m_model;
    std::weak_ptr<ITexture> m_texture;
    UniformValue<glm::mat4> m_position;

    std::array<InterfaceDescriptor, s_layout.size()> m_uniformDescriptors;
};

}    //  namespace renderer

#pragma once

#include "irenderable.hpp"

#include "shader_interface.hpp"
#include "uniform_value.hpp"

#include <glm/mat4x4.hpp>

class IShaderResourceProvider;

class Renderable
    : public IRenderable
    , public SIShaderInterfaceContainer<Renderable>
{
public:
    static constexpr ShaderInterfaceLayout<2> s_layout = {
        ShaderInterfaceBinding{
            .id = InterfaceBlockID::IBLOCK_ID_1,
            .type = ShaderBlockType::UNIFORM_DYNAMIC,
            .stage = ShaderStage::VERTEX,
        },
        ShaderInterfaceBinding{
            .id = InterfaceBlockID::IBLOCK_ID_2,
            .type = ShaderBlockType::SAMPLER,
            .stage = ShaderStage::FRAGMENT,
        },
    };

public:
    Renderable(IShaderResourceProvider& provider);
    Renderable(const Renderable& other) noexcept;
    Renderable(Renderable&& other) noexcept;
    virtual ~Renderable();
    virtual void draw(const RenderContext& context) const override;

    virtual void bind(RenderContext& context) override;
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

    std::weak_ptr<IPipeline::IBindContext> m_bindContext;
    std::array<InterfaceDescriptor, s_layout.size()> m_uniformDescriptors;
};

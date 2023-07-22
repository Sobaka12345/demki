#pragma once

#include "irenderable.hpp"

#include "uniform.hpp"
#include "uniform_value.hpp"

#include <glm/mat4x4.hpp>

class IUniformProvider;

class Renderable
    : public IRenderable
    , public SIUniformContainer<Renderable>
{
public:
    static constexpr std::array s_layout = {
        UniformBinding{
            .id = UniformID::POSITION,
            .type = UniformType::DYNAMIC,
            .stage = UniformStage::VERTEX,
        },
        UniformBinding{
            .id = UniformID::SAMPLER,
            .type = UniformType::SAMPLER,
            .stage = UniformStage::FRAGMENT,
        },
    };

public:
    Renderable(IUniformProvider& provider);
    Renderable(const Renderable& other) noexcept;
    Renderable(Renderable&& other) noexcept;
    virtual ~Renderable();
    virtual void draw(const RenderContext& context) const override;

    virtual void bind(RenderContext& context) override;
    virtual std::span<const UniformDescriptor> uniforms() const override;
    virtual std::span<const UniformDescriptor> dynamicUniforms() const override;

    virtual std::weak_ptr<IModel> model() const override;
    virtual void setModel(std::weak_ptr<IModel> model) override;

    virtual std::weak_ptr<ITexture> texture() const override;
    virtual void setTexture(std::weak_ptr<ITexture> texture) override;

    void setPosition(glm::mat4 position)
    {
        m_position.set(std::move(position));
        m_position.sync();
    }

    glm::mat4 position() const { return m_position.get(); }

private:
    IUniformProvider& m_uniformProvider;

    std::weak_ptr<IModel> m_model;
    std::weak_ptr<ITexture> m_texture;
    UniformValue<glm::mat4> m_position;

    std::shared_ptr<IPipeline::IBindContext> m_bindContext;
    std::array<UniformDescriptor, s_layout.size()> m_uniformDescriptors;
};

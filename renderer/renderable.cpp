#include "renderable.hpp"


#include <imodel.hpp>
#include <itexture.hpp>
#include <iresource_manager.hpp>

Renderable::Renderable(IShaderResourceProvider& provider)
    : m_shaderResourceProvider(provider)
    , m_position(provider.fetchHandle(ShaderBlockType::UNIFORM_DYNAMIC, m_position.s_layoutSize))
{
    m_uniformDescriptors[0].handle = m_position.handle();
    m_uniformDescriptors[0].binding = s_layout[0];
}

Renderable::Renderable(const Renderable& other) noexcept
    : m_shaderResourceProvider(other.m_shaderResourceProvider)
    , m_position(other.m_shaderResourceProvider.fetchHandle(ShaderBlockType::UNIFORM_DYNAMIC,
          m_position.s_layoutSize))
    , m_model(other.m_model)
    , m_texture(other.m_texture)
{
    m_uniformDescriptors[0].handle = m_position.handle();
    m_uniformDescriptors[0].binding = s_layout[0];
    if (!m_texture.expired())
    {
        m_uniformDescriptors[1].handle = m_texture.lock()->uniformHandle();
        m_uniformDescriptors[1].binding = s_layout[1];
    }
}

Renderable::Renderable(Renderable&& other) noexcept
    : m_shaderResourceProvider(other.m_shaderResourceProvider)
    , m_model(other.m_model)
    , m_texture(other.m_texture)
    , m_position(std::move(other.m_position))
    , m_uniformDescriptors(std::move(other.m_uniformDescriptors))
{}

Renderable::~Renderable() {}

void Renderable::draw(const OperationContext& context) const
{
    if (!m_model.expired() && !m_texture.expired())
    {
        m_model.lock()->draw(context);
    }
}

void Renderable::bind(OperationContext& context)
{
    if (m_model.expired() || m_texture.expired()) return;

    m_model.lock()->bind(context);
    m_texture.lock()->bind(context);

    IShaderInterfaceContainer::bind(context);
}

std::span<const IShaderInterfaceContainer::InterfaceDescriptor> Renderable::uniforms() const
{
    return m_uniformDescriptors;
}

std::span<const IShaderInterfaceContainer::InterfaceDescriptor> Renderable::dynamicUniforms() const
{
    return std::span{ m_uniformDescriptors.begin(), 1 };
}

std::weak_ptr<IModel> Renderable::model() const
{
    return m_model;
}

void Renderable::setModel(std::weak_ptr<IModel> model)
{
    m_model = model;
}

std::weak_ptr<ITexture> Renderable::texture() const
{
    return m_texture;
}

void Renderable::setTexture(std::weak_ptr<ITexture> texture)
{
    m_uniformDescriptors[1].handle = texture.lock()->uniformHandle();
    m_uniformDescriptors[1].binding = s_layout[1];

    m_texture = texture;
}

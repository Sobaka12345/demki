#pragma once

#include <itexture.hpp>

namespace ogl {

class GraphicsContext;

class Texture : public ITexture
{
public:
    explicit Texture(GraphicsContext& context, ITexture::CreateInfo createInfo) noexcept;
    virtual ~Texture();

    virtual void bind(OperationContext& context) override;
    virtual std::shared_ptr<IShaderInterfaceHandle> uniformHandle() override;

private:
    GraphicsContext& m_context;
};

}    //  namespace ogl

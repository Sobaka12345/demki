#pragma once

#include <itexture.hpp>
#include <iuniform_handle.hpp>

#include <span>
#include <cstdint>

class IModel;
class Vertex3DColoredTextured;

class IResourceManager : public IUniformProvider
{
public:
    struct CreateInfo
    {};

public:
    virtual std::shared_ptr<IModel> createModel(std::span<const Vertex3DColoredTextured> vertices,
        std::span<const uint32_t> indices) = 0;
    virtual std::shared_ptr<IModel> createModel(std::filesystem::path path) = 0;
    virtual std::shared_ptr<ITexture> createTexture(ITexture::CreateInfo createInfo) = 0;

    //  virtual createFactory() = 0;

    virtual ~IResourceManager() {}
};

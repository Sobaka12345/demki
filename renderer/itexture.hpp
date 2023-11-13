#pragma once

#include "iresource.hpp"

#include <filesystem>

class RenderContext;
class IShaderInterfaceHandle;

class ITexture : virtual public IResource
{
public:
    struct CreateInfo
    {
        std::filesystem::path path;
    };

public:
    virtual ~ITexture() {}

    virtual void bind(RenderContext& context) = 0;
    virtual std::shared_ptr<IShaderInterfaceHandle> uniformHandle() = 0;
};

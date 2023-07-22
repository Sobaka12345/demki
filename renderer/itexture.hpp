#pragma once

#include <filesystem>

class RenderContext;
class IUniformHandle;

class ITexture
{
public:
    struct CreateInfo
    {
        std::filesystem::path path;
    };

public:
    virtual ~ITexture() {}

    virtual void bind(RenderContext& context) = 0;
    virtual std::shared_ptr<IUniformHandle> uniformHandle() = 0;
};

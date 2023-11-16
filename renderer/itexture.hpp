#pragma once

#include "iresource.hpp"

#include <filesystem>

class OperationContext;
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

    virtual void bind(OperationContext& context) = 0;
    virtual std::shared_ptr<IShaderInterfaceHandle> uniformHandle() = 0;
};

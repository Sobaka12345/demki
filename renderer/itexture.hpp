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
        explicit CreateInfo(std::filesystem::path path);

        CreateInfo(const CreateInfo& other) = delete;

        CreateInfo(CreateInfo&& other);

        ~CreateInfo();

        void* pixels = nullptr;
        size_t imageSize;
        int textureChannels;
        int width;
        int height;
    };

public:
    virtual ~ITexture() {}

    virtual std::shared_ptr<IShaderInterfaceHandle> uniformHandle() = 0;
};

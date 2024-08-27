#pragma once

#include "ishader_resource.hpp"

#include <filesystem>

namespace renderer {

class OperationContext;
class IShaderInterfaceHandle;

class ITexture : virtual public IShaderResource
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
};

}    //  namespace renderer

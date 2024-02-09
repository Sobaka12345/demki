#pragma once

#include "../vertex.hpp"

#include <iresource.hpp>

#include <filesystem>
#include <span>

namespace renderer {

class OperationContext;

class IModel : public shell::IResource
{
public:
    struct CreateInfo
    {
        explicit CreateInfo(std::filesystem::path path);
        explicit CreateInfo(std::span<const Vertex3DColoredTextured> vertices,
            std::span<const uint32_t> indices);

        std::vector<Vertex3DColoredTextured> vertices;
        std::vector<uint32_t> indices;
    };

public:
    virtual ~IModel(){};

    virtual void bind(OperationContext& context) = 0;
    virtual void draw(OperationContext& context) = 0;
};

}    //  namespace renderer

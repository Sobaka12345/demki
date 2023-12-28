#pragma once

#include "iresource.hpp"

#include <vertex.hpp>

#include <span>


class OperationContext;

class IModel : public IResource
{
public:
    struct CreateInfo
    {
        std::span<const Vertex3DColoredTextured> vertices;
        std::span<const uint32_t> indices;
    };

public:
    virtual ~IModel(){};

    virtual void bind(OperationContext& context) = 0;
    virtual void draw(OperationContext& context) = 0;
};

#pragma once

#include "ibuffer.hpp"

#include "ishader_resource.hpp"

namespace renderer {

class IUniformBuffer
    : public IBuffer
    , public IShaderResource
{
public:
    struct CreateInfo : public IBuffer::CreateInfo<CreateInfo>
    {
    };

protected:
    IUniformBuffer()
        : IShaderResource(IResource::TYPE_UNIFORM_BUFFER)
    {}
};

}    //  namespace renderer

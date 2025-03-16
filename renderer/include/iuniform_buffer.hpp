#pragma once

#include "ibuffer.hpp"

#include "ishader_resource.hpp"

namespace renderer {

class IUniformBuffer
    : public IBuffer
    , public IShaderResource
{
public:
    struct CreateInfo : public IBuffer::CreateInfoBase<CreateInfo>
    {
    };
};

}    //  namespace renderer

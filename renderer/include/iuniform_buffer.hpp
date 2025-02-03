#pragma once

#include "ibuffer.hpp"

#include "ishader_resource.hpp"

namespace renderer {

class IUniformBuffer
    : public IBuffer
    , public IShaderResource
{
public:
    struct CreateInfo : public IBuffer::CreateInfo
    {
        CREATE_INFO_PROPERTY(bool, dynamic, false)
    };
};

}    //  namespace renderer

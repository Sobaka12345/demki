#pragma once

#include "ibuffer.hpp"

#include "ishader_resource.hpp"

namespace renderer {

class IUniformBuffer
    : public IBuffer
    , public IShaderResource
{
public:
    struct CreateInfo
    {
        bool isDynamic = false;
        size_t size = 0;
    };
};


}    //  namespace renderer

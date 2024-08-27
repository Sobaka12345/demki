#pragma once

#include "ishader_interface_handle.hpp"

#include <iresource.hpp>

namespace renderer {

struct IShaderResource : public shell::IResource
{
    virtual std::shared_ptr<IShaderInterfaceHandle> handle() = 0;
};

}

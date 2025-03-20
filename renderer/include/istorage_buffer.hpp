#pragma once

#include "../utils.hpp"

#include "ibuffer.hpp"
#include "icompute_target.hpp"
#include "ishader_resource.hpp"

#include <boost/pfr.hpp>

namespace renderer {

class IShaderInterfaceHandle;

class IStorageBuffer
    : public IComputeTarget
    , public IBuffer
    , public IShaderResource
{
public:
    struct CreateInfo : public IBuffer::CreateInfo<CreateInfo>
    {
        CREATE_INFO_PROPERTY(bool, normalized, false)
        CREATE_INFO_PROPERTY(StructMetaInfo, dataTypeMetaInfo, {})
    };

public:
    virtual ~IStorageBuffer() {}

    virtual void draw(OperationContext& context) const = 0;

protected:
    IStorageBuffer()
        : IShaderResource(IResource::TYPE_STORAGE_BUFFER)
    {}
};

}    //  namespace renderer

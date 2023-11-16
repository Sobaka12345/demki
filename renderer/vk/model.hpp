#pragma once

#include "vk/resource_manager.hpp"
#include "vk/handles/buffer.hpp"

#include <imodel.hpp>
#include <operation_context.hpp>

#include <span>
#include <memory>
#include <vector>

namespace vk {

class Model
    : public IModel
    , public vk::Resource
{
public:
    friend class ResourceManager;

    struct Descriptor : public vk::Resource::Descriptor
    {
        vk::handles::Buffer::Descriptor vertices;
        vk::handles::Buffer::Descriptor indices;
    };

public:
    Model(Descriptor descriptor);

    virtual void draw(const ::OperationContext& context) override;
    virtual void bind(const ::OperationContext& context) override;

private:
    Descriptor m_descriptor;
};

}    //  namespace vk

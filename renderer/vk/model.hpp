#pragma once

#include <imodel.hpp>
#include <operation_context.hpp>

#include <span>
#include <memory>
#include <vector>

namespace renderer::vk {

namespace handles {
class Memory;
}

class GraphicsContext;

class Model : public IModel
{
public:
    friend class ResourceManager;

public:
    Model(GraphicsContext& context, IModel::CreateInfo createInfo);

    virtual void draw(renderer::OperationContext& context) override;
    virtual void bind(renderer::OperationContext& context) override;

private:
    GraphicsContext& m_context;

    VkDeviceSize m_vertexSize;
    VkDeviceSize m_indexSize;
    VkDeviceSize m_indicesSize;
    VkDeviceSize m_verticesSize;

    std::weak_ptr<handles::Memory> m_memory;
};

}    //  namespace renderer::vk

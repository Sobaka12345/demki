#pragma once

#include <imesh.hpp>
#include <operation_context.hpp>

#include <memory>

namespace renderer::vk {

namespace handles {
class Memory;
}

class GraphicsContext;

class Mesh : public IMesh
{
public:
    friend class ResourceManager;

public:
    Mesh(GraphicsContext& context, IMesh::CreateInfo createInfo);

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

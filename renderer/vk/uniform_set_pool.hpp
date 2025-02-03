#pragma once

#include <iuniform_set_pool.hpp>

#include "handles/buffer.hpp"
#include "handles/descriptor_pool.hpp"
#include "handles/descriptor_set_layout.hpp"

#include <memory>

namespace renderer {

class OperationContext;

namespace vk {

class GraphicsContext;

class UniformSetPool : public IUniformSetPoolBase
{
public:
    UniformSetPool(GraphicsContext& context, IUniformSetPool::CreateInfo createInfo);

    virtual std::span<IUniformSet> spawnImpl(size_t count) noexcept override;
    virtual void bind(OperationContext& context) noexcept override = 0;

private:
    GraphicsContext& m_context;
    handles::DescriptorSetLayout m_layout;
    size_t m_dynamicDescriptorIndex;

    std::multimap<ShaderBlockType, ShaderInterfaceBinding> m_bindingsByType;
    std::map<ShaderBlockType, std::pair<uint32_t, handles::Buffer>> m_buffers;

    std::vector<uint32_t> m_dynamicOffsets;
    std::vector<VkDescriptorSet> m_descriptorSets;

    handles::DescriptorPool m_pool;
};

}
}

#include "uniform_set_pool.hpp"

#include "pipeline.hpp"
#include "graphics_context.hpp"

#include "handles/buffer.hpp"
#include "handles/descriptor_pool.hpp"
#include "handles/memory.hpp"
#include "handles/pipeline_layout.hpp"

#include <algorithm>

namespace renderer::vk {

constexpr int c_swapchainSize = 2; // swapchain framebuffer count : TEMPORARY

UniformSetPool::UniformSetPool(GraphicsContext &context, CreateInfo createInfo)
    : m_context(context)
    , m_dynamicDescriptorIndex(0)
    , m_layout(context.descriptorSetLayout(createInfo.shaderInterfaceContainer.id()))
{
    std::vector<handles::DescriptorPoolSize> poolSizes;
    poolSizes.reserve(static_cast<size_t>(ShaderBlockType::COUNT));

    uint32_t dynamicUniformBufferSize = 0;
    uint32_t staticUniformBufferSize = 0;
    uint32_t storageBufferSize = 0;
    std::vector<handles::WriteDescriptorSet> writes;

    auto bindings = createInfo.shaderInterfaceContainer.layout();
    for (auto& binding : bindings) m_bindingsByType.emplace(binding.type, binding);

    for (ShaderBlockType type = ShaderBlockType::BEGIN; type < ShaderBlockType::COUNT; ++type)
    {
        auto [begin, end] = m_bindingsByType.equal_range(type);
        if (begin == end) continue;

        uint32_t descriptorCount = 0;
        uint32_t bufferUnitSize = 0;

        while (begin++ != end) {
            auto& binding = begin->second;
            descriptorCount += binding.count * createInfo.size;
            bufferUnitSize += binding.size;
        }
        poolSizes.emplace_back(handles::DescriptorPoolSize{}
            .descriptorCount(descriptorCount)
            .type(toDescriptorType(type))
        );

        VkBufferUsageFlags usage = 0;
        uint32_t mult = createInfo.size;
        const uint32_t alignment = m_context.dynamicAlignment(bufferUnitSize);

        switch (type) {
        case renderer::ShaderBlockType::UNIFORM_DYNAMIC:
            mult *= c_swapchainSize;
            m_dynamicOffsets.resize(mult, 0);
        case renderer::ShaderBlockType::UNIFORM_STATIC:
            usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
            break;
        case renderer::ShaderBlockType::STORAGE:
            usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
            break;
        }

        auto [iter, _] = m_buffers.emplace(std::piecewise_construct,
            std::forward_as_tuple(type),
            std::forward_as_tuple(
                alignment,
                m_context.device(), handles::BufferCreateInfo{}
                   .size(m_context.dynamicAlignment(bufferUnitSize) * mult)
                   .usage(usage)
                   .sharingMode(VK_SHARING_MODE_EXCLUSIVE)
            )
        );

        auto& buffer = iter->second.second;
        buffer.allocateAndBindMemory(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT).lock()->map();
    }

    m_pool = std::make_unique<handles::DescriptorPool>(m_context.device(),
        handles::DescriptorPoolCreateInfo{}
            .maxSets(createInfo.size)
            .poolSizeCount(poolSizes.size())
            .pPoolSizes(poolSizes.data())
            .flags(VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT)
    );
}

std::span<IUniformSet> UniformSetPool::spawnImpl(size_t count) noexcept
{
    const auto allocInfo = handles::DescriptorSetAllocateInfo{}
        .descriptorPool(*m_pool)
        .descriptorSetCount(count)
        .pSetLayouts(m_layout.handlePtr());

    const size_t oldSize = m_descriptorSets.size();
    const size_t newSize = m_descriptorSets.size() + count;
    m_descriptorSets.resize(newSize);
    vkAllocateDescriptorSets(m_context.device(), &allocInfo, m_descriptorSets.data() + oldSize);

    std::vector<handles::WriteDescriptorSet> writeSet;
    writeSet.reserve(newSize - oldSize);
    for (size_t i = oldSize; i < newSize; ++i)
    {
        for (ShaderBlockType type = ShaderBlockType::BEGIN; type < ShaderBlockType::COUNT; ++type)
        {
            auto [begin, end] = m_bindingsByType.equal_range(type);
            if (begin == end) continue;

            auto& [alignment, buffer] = m_buffers[type];

        }
    }

    for (size_t i = 0; i < c_swapchainSize; ++i) {
        //m_dynamicOffsets
    }
}

void UniformSetPool::bind(renderer::OperationContext& context) noexcept
{
    auto& specContext = get(context);
    const ISpecificPipeline* pipeline = specContext.specificPipeline();
    DASSERT(pipeline, "pipeline is null");

    vkCmdBindDescriptorSets(specContext.commandBuffer->handle(), pipeline->bindPoint(), pipeline->layout(), 0, m_descriptorSets.size(), m_descriptorSets.data(),
        m_dynamicOffsets.size() / c_swapchainSize, m_dynamicOffsets.data() + m_dynamicDescriptorIndex * m_dynamicOffsets.size());
    m_dynamicDescriptorIndex = (m_dynamicDescriptorIndex + 1) % c_swapchainSize;
}

}

#include "descriptor_set.hpp"

#include "device.hpp"
#include "descriptor_pool.hpp"

namespace vk { namespace handles {

DescriptorSet::DescriptorSet(DescriptorSet&& other)
    : Handle(std::move(other))
    , m_device(other.m_device)
    , m_pool(std::move(other.m_pool))
{}

DescriptorSet::DescriptorSet(const Device& device,
    HandlePtr<DescriptorPool> pool,
    std::span<const VkDescriptorSetLayout> setLayouts,
    VkHandleType* handlePtr)
    : Handle(handlePtr)
    , m_device(device)
    , m_pool(pool)
{
    auto info =
        DescriptorSetAllocateInfo{}
            .descriptorPool(m_pool->handle())
            .descriptorSetCount(setLayouts.size())
            .pSetLayouts(setLayouts.data());
    ASSERT(create(vkAllocateDescriptorSets, m_device, &info) == VK_SUCCESS,
        "failed to allocate descriptor sets!");
}

DescriptorSet::~DescriptorSet()
{
    if (m_pool.isAlive()) destroy(vkFreeDescriptorSets, m_device, m_pool->handle(), 1, handlePtr());
}

void DescriptorSet::write(std::span<const Write> writes)
{
    std::vector<WriteDescriptorSet> writeDescriptorSets;
    for (auto& write : writes)
    {
        writeDescriptorSets.push_back(
            WriteDescriptorSet()
                .dstSet(handle())
                .dstBinding(write.layoutBinding.binding)
                .descriptorType(write.layoutBinding.descriptorType)
                .descriptorCount(write.layoutBinding.descriptorCount)
                .pImageInfo(write.imageInfo.has_value() ? &write.imageInfo.value() : nullptr)
                .pBufferInfo(write.bufferInfo.has_value() ? &write.bufferInfo.value() : nullptr));
    }

    write(writeDescriptorSets);
}

void DescriptorSet::write(std::span<const WriteDescriptorSet> writes)
{
    vkUpdateDescriptorSets(m_device, static_cast<uint32_t>(writes.size()), writes.data(), 0,
        nullptr);
}

void DescriptorSets::write(std::span<const WriteDescriptorSet> writes)
{
    DASSERT(size());
    vkUpdateDescriptorSets((*this)[0].m_device, static_cast<uint32_t>(writes.size()), writes.data(),
        0, nullptr);
}

}}    //  namespace vk::handles

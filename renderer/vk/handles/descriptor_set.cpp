#include "descriptor_set.hpp"

#include "device.hpp"
#include "descriptor_pool.hpp"
#include "descriptor_set_layout.hpp"

namespace renderer::vk { namespace handles {

HandleVector<DescriptorSet> DescriptorSet::create(const Device& device,
    HandlePtr<DescriptorPool> pool,
    const HandleVector<DescriptorSetLayout>& setLayouts)
{
    const auto allocateInfo =
        DescriptorSetAllocateInfo{}
            .descriptorPool(pool->handle())
            .descriptorSetCount(setLayouts.size())
            .pSetLayouts(setLayouts.handleData());
    HandleVector<DescriptorSet> result;

    result.emplaceBackBatch(vkAllocateDescriptorSets, allocateInfo.descriptorSetCount(),
        std::forward_as_tuple(device.handle(), &allocateInfo), std::forward_as_tuple(device, pool));

    return result;
}

std::vector<std::shared_ptr<DescriptorSet>> DescriptorSet::createShared(const Device& device,
    HandlePtr<DescriptorPool> pool,
    const HandleVector<DescriptorSetLayout>& setLayouts,
    std::function<void(DescriptorSet*)> destructor)
{
    const auto allocateInfo =
        DescriptorSetAllocateInfo{}
            .descriptorPool(pool->handle())
            .descriptorSetCount(setLayouts.size())
            .pSetLayouts(setLayouts.handleData());

    std::vector<VkDescriptorSet> allocatedSets;
    allocatedSets.resize(allocateInfo.descriptorSetCount());
    ASSERT(vkAllocateDescriptorSets(device, &allocateInfo, allocatedSets.data()) == VK_SUCCESS,
        "failed to allocate descriptor sets");

    std::vector<std::shared_ptr<DescriptorSet>> result;
    result.reserve(allocateInfo.descriptorSetCount());
    for (auto vkSet : allocatedSets)
        result.emplace_back(new DescriptorSet{ device, pool, vkSet }, destructor)->setOwner(true);

    return result;
}

DescriptorSet::DescriptorSet(DescriptorSet&& other) noexcept
    : Handle(std::move(other))
    , m_device(other.m_device)
    , m_pool(std::move(other.m_pool))
{}

DescriptorSet::DescriptorSet(
    const Device& device, HandlePtr<DescriptorPool> pool, VkHandleType* handlePtr) noexcept
    : Handle(handlePtr)
    , m_device(device)
    , m_pool(pool)
{}

DescriptorSet::DescriptorSet(
    const Device& device, HandlePtr<DescriptorPool> pool, VkHandleType handle) noexcept
    : Handle(handle)
    , m_device(device)
    , m_pool(pool)
{}

DescriptorSet::DescriptorSet(const Device& device,
    HandlePtr<DescriptorPool> pool,
    const DescriptorSetLayout& setLayout,
    VkHandleType* handlePtr) noexcept
    : DescriptorSet(device, pool, handlePtr)
{
    auto allocateInfo =
        DescriptorSetAllocateInfo{}
            .descriptorPool(pool->handle())
            .descriptorSetCount(1)
            .pSetLayouts(setLayout.handlePtr());

    ASSERT(Handle::create(vkAllocateDescriptorSets, m_device, &allocateInfo) == VK_SUCCESS,
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

}}    //  namespace renderer::vk::handles

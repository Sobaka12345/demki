#pragma once

#include "handle.hpp"

#include "vk/utils.hpp"

#include <functional>
#include <memory>
#include <span>
#include <optional>

namespace renderer::vk { namespace handles {

BEGIN_DECLARE_VKSTRUCT(DescriptorSetAllocateInfo, VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO)
    VKSTRUCT_PROPERTY(const void*, pNext)
    VKSTRUCT_PROPERTY(VkDescriptorPool, descriptorPool)
    VKSTRUCT_PROPERTY(uint32_t, descriptorSetCount)
    VKSTRUCT_PROPERTY(const VkDescriptorSetLayout*, pSetLayouts)
END_DECLARE_VKSTRUCT();

BEGIN_DECLARE_VKSTRUCT(WriteDescriptorSet, VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET)
    VKSTRUCT_PROPERTY(const void*, pNext)
    VKSTRUCT_PROPERTY(VkDescriptorSet, dstSet)
    VKSTRUCT_PROPERTY(uint32_t, dstBinding)
    VKSTRUCT_PROPERTY(uint32_t, dstArrayElement)
    VKSTRUCT_PROPERTY(uint32_t, descriptorCount)
    VKSTRUCT_PROPERTY(VkDescriptorType, descriptorType)
    VKSTRUCT_PROPERTY(const VkDescriptorImageInfo*, pImageInfo)
    VKSTRUCT_PROPERTY(const VkDescriptorBufferInfo*, pBufferInfo)
    VKSTRUCT_PROPERTY(const VkBufferView*, pTexelBufferView)
END_DECLARE_VKSTRUCT()

class Device;
class DescriptorPool;
class DescriptorSetLayout;

class DescriptorSet : public Handle<VkDescriptorSet>
{
    HANDLE(DescriptorSet);
    friend class DescriptorSets;

public:
    struct Write
    {
        std::optional<VkDescriptorImageInfo> imageInfo;
        std::optional<VkDescriptorBufferInfo> bufferInfo;
        VkDescriptorSetLayoutBinding layoutBinding;
    };

public:
    DescriptorSet(const DescriptorSet& other) = delete;
    DescriptorSet(DescriptorSet&& other) noexcept;
    virtual ~DescriptorSet();

    void write(std::span<const Write> writes);
    void write(std::span<const WriteDescriptorSet> writes);

protected:
    static HandleVector<DescriptorSet> create(const Device& device,
        HandlePtr<DescriptorPool> pool,
        const HandleVector<DescriptorSetLayout>& setLayouts);

    static std::vector<std::shared_ptr<DescriptorSet>> createShared(const Device& device,
        HandlePtr<DescriptorPool> pool,
        const HandleVector<DescriptorSetLayout>& setLayouts,
        std::function<void(DescriptorSet*)> destructor = std::default_delete<DescriptorSet>{});

protected:
    DescriptorSet(const Device& device,
        HandlePtr<DescriptorPool> pool,
        const DescriptorSetLayout& setLayout,
        VkHandleType* handlePtr = nullptr) noexcept;

    DescriptorSet(
        const Device& device, HandlePtr<DescriptorPool> pool, VkHandleType* handlePtr) noexcept;

    DescriptorSet(
        const Device& device, HandlePtr<DescriptorPool> pool, VkHandleType handle) noexcept;

    friend class DescriptorPool;

protected:
    const Device& m_device;
    HandlePtr<DescriptorPool> m_pool;
};

}}    //  namespace renderer::vk::handles

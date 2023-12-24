#pragma once

#include "handle.hpp"

#include "vk/utils.hpp"

namespace vk { namespace handles {

BEGIN_DECLARE_VKSTRUCT(PipelineLayoutCreateInfo, VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO)
    VKSTRUCT_PROPERTY(const void*, pNext)
    VKSTRUCT_PROPERTY(VkPipelineLayoutCreateFlags, flags)
    VKSTRUCT_PROPERTY(uint32_t, setLayoutCount)
    VKSTRUCT_PROPERTY(const VkDescriptorSetLayout*, pSetLayouts)
    VKSTRUCT_PROPERTY(uint32_t, pushConstantRangeCount)
    VKSTRUCT_PROPERTY(const VkPushConstantRange*, pPushConstantRanges)
END_DECLARE_VKSTRUCT()

class Device;

class PipelineLayout : public Handle<VkPipelineLayout>
{
    HANDLE(PipelineLayout);

public:
    PipelineLayout(const PipelineLayout& other) = delete;
    PipelineLayout(PipelineLayout&& other) noexcept;
    PipelineLayout(const Device& device, PipelineLayoutCreateInfo createInfo) noexcept;
    virtual ~PipelineLayout();

protected:
    PipelineLayout(const Device& device,
        PipelineLayoutCreateInfo createInfo,
        VkHandleType* handlePtr) noexcept;

private:
    const Device& m_device;
};

}}    //  namespace vk::handles

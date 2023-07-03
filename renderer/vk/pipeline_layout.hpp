#pragma once

#include "utils.hpp"

namespace vk {

BEGIN_DECLARE_VKSTRUCT(PipelineLayoutCreateInfo, VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO)
VKSTRUCT_PROPERTY(const void*                 , pNext)
VKSTRUCT_PROPERTY(VkPipelineLayoutCreateFlags , flags)
VKSTRUCT_PROPERTY(uint32_t                    , setLayoutCount)
VKSTRUCT_PROPERTY(const VkDescriptorSetLayout*, pSetLayouts)
VKSTRUCT_PROPERTY(uint32_t                    , pushConstantRangeCount)
VKSTRUCT_PROPERTY(const VkPushConstantRange*  , pPushConstantRanges)
END_DECLARE_VKSTRUCT()

class Device;

class PipelineLayout : public Handle<VkPipelineLayout>
{
public:
    PipelineLayout(const PipelineLayout& other) = delete;
    PipelineLayout(PipelineLayout&& other);
    PipelineLayout(const Device& device, PipelineLayoutCreateInfo createInfo,
        VkHandleType* handlePtr = nullptr);
    ~PipelineLayout();

private:
    const Device& m_device;
};

}

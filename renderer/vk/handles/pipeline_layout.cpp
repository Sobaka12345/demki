#include "pipeline_layout.hpp"

#include "device.hpp"

namespace vk { namespace handles {

PipelineLayout::PipelineLayout(PipelineLayout &&other)
    : Handle(std::move(other))
    , m_device(other.m_device)
{}

PipelineLayout::PipelineLayout(
    const Device &device, PipelineLayoutCreateInfo createInfo, VkHandleType *handlePtr)
    : Handle(handlePtr)
    , m_device(device)
{
    ASSERT(create(vkCreatePipelineLayout, m_device, &createInfo, nullptr) == VK_SUCCESS,
        "failed to create pipeline layout");
}

PipelineLayout::~PipelineLayout()
{
    destroy(vkDestroyPipelineLayout, m_device, handle(), nullptr);
}

}}    //  namespace vk::handles

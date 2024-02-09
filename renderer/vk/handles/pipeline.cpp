#include "pipeline.hpp"

#include "device.hpp"

namespace renderer::vk { namespace handles {
Pipeline::Pipeline(Pipeline&& other) noexcept
    : Handle(std::move(other))
    , m_device(other.m_device)
    , m_cache(other.m_cache)
{
    other.m_cache = VK_NULL_HANDLE;
}

Pipeline::Pipeline(const Device& device, VkPipelineCache cache, VkHandleType* handlePtr) noexcept
    : Handle(handlePtr)
    , m_device(device)
    , m_cache(cache)
{}

Pipeline::~Pipeline()
{
    destroy(vkDestroyPipeline, m_device, handle(), nullptr);
}

}}    //  namespace renderer::vk::handles

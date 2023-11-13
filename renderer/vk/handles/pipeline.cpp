#include "pipeline.hpp"

#include "device.hpp"

namespace vk { namespace handles {
Pipeline::Pipeline(Pipeline&& other)
    : Handle(std::move(other))
    , m_device(other.m_device)
    , m_cache(other.m_cache)
{}

Pipeline::Pipeline(const Device& device, VkPipelineCache cache, VkHandleType* handlePtr)
    : Handle(handlePtr)
    , m_device(device)
    , m_cache(cache)
{}

Pipeline::~Pipeline()
{
    destroy(vkDestroyPipeline, m_device, handle(), nullptr);
}

}}    //  namespace vk::handles

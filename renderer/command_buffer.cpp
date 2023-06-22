#include "command_buffer.hpp"


namespace vk {
CommandBuffer::CommandBuffer(const Device& device, VkHandleType* handlePtr)
    : Handle(handlePtr)
    , m_device(device)
{
}

CommandBuffer::~CommandBuffer()
{
}

CommandBuffer::CommandBuffer(CommandBuffer &&other) noexcept
    : Handle(std::move(other))
    , m_device(other.m_device)
{}

}

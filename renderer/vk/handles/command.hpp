#pragma once

#include "command_buffer.hpp"

#include <functional>

namespace vk { namespace handles {

class Queue;
class Device;
enum QueueFamilyType : uint16_t;

class OneTimeCommand
{
public:
	OneTimeCommand(const Queue& queue, const CommandPool& pool);
	OneTimeCommand(const Device& device, QueueFamilyType type, uint32_t queueIdx = 0);

	~OneTimeCommand();

	const CommandBuffer& operator()(void);

private:
	const Queue& m_queue;
	const CommandPool& m_pool;
	CommandBuffer m_commandBuffer;
};

}}    //  namespace vk::handles

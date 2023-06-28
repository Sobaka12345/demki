#include "command.hpp"
#include "command_pool.hpp"
#include "device.hpp"
#include "queue.hpp"

namespace vk {

OneTimeCommand::OneTimeCommand(const Queue& queue, const CommandPool& pool)
	: m_queue(queue)
	, m_pool(pool)
	, m_commandBuffer(m_pool.allocateBuffer())
{
	m_commandBuffer.setOwner(true);
	m_commandBuffer.begin(
		create::commandBufferBeginInfo(nullptr, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT));
}

OneTimeCommand::OneTimeCommand(const Device& device, QueueFamilyType type, uint32_t queueIdx)
	: OneTimeCommand(*device.queue(type, queueIdx).lock(), *device.commandPool(type).lock())
{
}

OneTimeCommand::~OneTimeCommand()
{
	m_commandBuffer.end();

	const std::array submitBuffers = { m_commandBuffer.handle() };
	const auto submitInfo = create::submitInfo(submitBuffers);

	m_queue.submit(1, &submitInfo, VK_NULL_HANDLE);
	m_queue.waitIdle();
}

const CommandBuffer& OneTimeCommand::operator()(void)
{
	return m_commandBuffer;
}

};
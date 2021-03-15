#pragma once

#include <vulkan/vulkan.h>
#include <cstdint>

namespace symphony
{
	class Queue
	{
	public:
		Queue(const VkDevice& device, uint32_t queueIndex);
		~Queue();

		const VkQueue& queue() const {
			return m_QueueHandle;
		}

		operator const VkQueue& () const {
			return m_QueueHandle;
		}
	private:
		VkQueue m_QueueHandle;
	};
}
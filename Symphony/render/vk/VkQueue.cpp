#include "VkQueue.h"

namespace symphony
{
	Queue::Queue(const VkDevice& device, uint32_t queueIndex)
	{
		vkGetDeviceQueue(device, queueIndex, 0, &m_QueueHandle);
	}

	Queue::~Queue()
	{

	}
}
#pragma once

#include <vulkan/vulkan.h>

namespace symphony
{
	class CommandPool
	{
	public:
		CommandPool(const VkDevice& device, uint32_t graphicsQueueIndex);
		~CommandPool();

		const VkCommandPool& command_pool() const {
			return m_CommandPool;
		}

		operator VkCommandPool() const {
			return m_CommandPool;
		}

	private:
		const VkDevice& m_DeviceCopy;
		VkCommandPool m_CommandPool;
	};
}
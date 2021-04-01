#include "VkCommandPool.h"
#include "core/exception/VulkanException.h"

namespace symphony
{
	CommandPool::CommandPool(const VkDevice& device, uint32_t graphicsQueueIndex)
		: m_DeviceCopy(device)
	{
		VkCommandPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolInfo.queueFamilyIndex = graphicsQueueIndex;
		poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT; 

		if (vkCreateCommandPool(device, &poolInfo, nullptr, &m_CommandPool) != VK_SUCCESS) {
			throw VulkanException("failed to create command pool!");
		}
	}

	CommandPool::~CommandPool()
	{
		vkDestroyCommandPool(m_DeviceCopy, m_CommandPool, nullptr);
	}
}
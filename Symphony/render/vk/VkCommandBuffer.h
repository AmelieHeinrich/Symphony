#pragma once

#include <vulkan/vulkan.h>
#include "VkCommandPool.h"
#include <memory>
#include "VkDevice.h"

namespace symphony
{
	// That took forever to make
	class CommandBuffer
	{
	public:
		CommandBuffer(std::shared_ptr<Device> device, std::shared_ptr<CommandPool> commandPool, bool begin = true, VkQueueFlagBits queueType = VK_QUEUE_GRAPHICS_BIT, VkCommandBufferLevel bufferLevel = VK_COMMAND_BUFFER_LEVEL_PRIMARY);
		~CommandBuffer();

		void Begin(VkCommandBufferUsageFlags usage = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
		void End();

		void SubmitIdle();
		void Submit(const VkSemaphore& waitSemaphore = VK_NULL_HANDLE, const VkSemaphore& signalSemaphore = VK_NULL_HANDLE, VkFence fence = VK_NULL_HANDLE);

		static VkCommandBuffer BeginSingleTimeCommands();
		static void EndSingleTimeCommands(VkCommandBuffer buffer);

		operator const VkCommandBuffer& () const { return commandBuffer; }

		const VkCommandBuffer& GetCommandBuffer() const { return commandBuffer; }
	private:
		std::shared_ptr<Device> m_DeviceCopy;
		VkQueue GetQueue() const;

		std::shared_ptr<CommandPool> commandPool;

		VkQueueFlagBits queueType;
		VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
	};
}
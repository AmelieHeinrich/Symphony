#include "VkCommandBuffer.h"
#include "core/exception/VulkanException.h"

namespace symphony
{
	CommandBuffer::CommandBuffer(std::shared_ptr<Device> device, std::shared_ptr<CommandPool> commandPool, bool begin, VkQueueFlagBits queueType, VkCommandBufferLevel bufferLevel)
		: m_DeviceCopy(device), commandPool(commandPool), queueType(queueType)
	{
		VkCommandBufferAllocateInfo commandBufferAllocateInfo = {};
		commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		commandBufferAllocateInfo.commandPool = *commandPool;
		commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		commandBufferAllocateInfo.commandBufferCount = 1;

		if (vkAllocateCommandBuffers(m_DeviceCopy->device(), &commandBufferAllocateInfo, &commandBuffer) != VK_SUCCESS) {
			throw VulkanException("Failed to allocate command buffers!");
		}

		if (begin)
			Begin();
	}

	CommandBuffer::~CommandBuffer()
	{
		vkFreeCommandBuffers(m_DeviceCopy->device(), commandPool->command_pool(), 1, &commandBuffer);
	}

	void CommandBuffer::Begin(VkCommandBufferUsageFlags usage)
	{
		if (running)
			return;

		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.pInheritanceInfo = nullptr;
		beginInfo.flags = usage;

		if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
			throw VulkanException("Failed to begin command buffer!");
		}
		running = true;
	}

	void CommandBuffer::End()
	{
		if (!running) return;

		if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
			throw VulkanException("Failed to end command buffer!");
		}

		running = false;
	}

	void CommandBuffer::SubmitIdle()
	{
		auto queueSelected = GetQueue();

		if (running)
			End();

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		VkFenceCreateInfo fenceCreateInfo = {};
		fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

		VkFence fence;
		if (vkCreateFence(m_DeviceCopy->device(), &fenceCreateInfo, nullptr, &fence) != VK_SUCCESS &&
			vkResetFences(m_DeviceCopy->device(), 1, &fence) &&
			vkQueueSubmit(queueSelected, 1, &submitInfo, fence) &&
			vkWaitForFences(m_DeviceCopy->device(), 1, &fence, VK_TRUE, std::numeric_limits<uint64_t>::max())) {
			throw VulkanException("Failed to submit idle fence");
		}

		vkDestroyFence(m_DeviceCopy->device(), fence, nullptr);
	}

	void CommandBuffer::Submit(const VkSemaphore& waitSemaphore, const VkSemaphore& signalSemaphore, VkFence fence)
	{
		auto queueSelected = GetQueue();

		if (running)
			End();

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		if (waitSemaphore != VK_NULL_HANDLE) {
			// Pipeline stages used to wait at for graphics queue submissions.
			static VkPipelineStageFlags submitPipelineStages = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

			submitInfo.pWaitDstStageMask = &submitPipelineStages;
			submitInfo.waitSemaphoreCount = 1;
			submitInfo.pWaitSemaphores = &waitSemaphore;
		}

		if (signalSemaphore != VK_NULL_HANDLE) {
			submitInfo.signalSemaphoreCount = 1;
			submitInfo.pSignalSemaphores = &signalSemaphore;
		}

		if (fence != VK_NULL_HANDLE)
			vkResetFences(m_DeviceCopy->device(), 1, &fence);

		vkQueueSubmit(queueSelected, 1, &submitInfo, fence);
	}

	VkQueue CommandBuffer::GetQueue() const
	{
		switch (queueType) {
		case VK_QUEUE_GRAPHICS_BIT:
			return m_DeviceCopy->graphics_queue()->queue();
		case VK_QUEUE_COMPUTE_BIT:
			return m_DeviceCopy->present_queue()->queue();
		default:
			return nullptr;
		}
	}
}
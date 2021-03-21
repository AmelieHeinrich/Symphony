#pragma once

#include <vulkan/vulkan.h>

namespace symphony
{
	struct RendererUniforms;

	class VulkanUniformBuffer
	{
	public:
		VulkanUniformBuffer();
		~VulkanUniformBuffer();

		// Right now UniformBuffers only use the RendererUniforms...
		// TODO : Custom uniform buffers
		void Update(RendererUniforms uniform);

		const VkBuffer& GetBuffer() const {
			return uniformBuffer;
		}

		const VkDeviceMemory& GetDeviceMemory() const {
			return uniformBufferMemory;
		}
	private:
		VkBuffer uniformBuffer;
		VkDeviceMemory uniformBufferMemory;
	};
}
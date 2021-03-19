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
#include "VkUniformBuffer.h"
#include "render/Renderer.h"
#include "VkVertexBuffer.h"
#include "VkRenderer.h"

namespace symphony
{
	VulkanUniformBuffer::VulkanUniformBuffer()
	{
        VkDeviceSize bufferSize = sizeof(RendererUniforms);

		VulkanVertexBuffer::CreateBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, uniformBuffer, uniformBufferMemory);
	}

	VulkanUniformBuffer::~VulkanUniformBuffer()
	{
		auto device = VulkanRenderer::GetData().m_Device->device();
		vkDestroyBuffer(device, uniformBuffer, nullptr);
		vkFreeMemory(device, uniformBufferMemory, nullptr);
	}

	void VulkanUniformBuffer::Update(RendererUniforms uniform)
	{
		auto device = VulkanRenderer::GetData().m_Device->device();

		void* data;
		vkMapMemory(device, uniformBufferMemory, 0, sizeof(uniform), 0, &data);
		memcpy(data, &uniform, sizeof(uniform));
		vkUnmapMemory(device, uniformBufferMemory);
	}
}

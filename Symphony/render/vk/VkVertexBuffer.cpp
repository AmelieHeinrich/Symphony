#include "VkVertexBuffer.h"
#include "VkRenderer.h"

namespace symphony
{
	VulkanVertexBuffer::VulkanVertexBuffer(const std::vector<Vertex>& vertices)
        : VertexBuffer(vertices)
	{
        const VkDevice& device = VulkanRenderer::GetData().m_Device->device();

        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = sizeof(vertices[0]) * vertices.size();
        bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateBuffer(device, &bufferInfo, nullptr, &m_BufferHandle) != VK_SUCCESS) {
            throw std::runtime_error("failed to create vertex buffer!");
        }

        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(device, m_BufferHandle, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = VulkanRenderer::FindMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        if (vkAllocateMemory(device, &allocInfo, nullptr, &m_VertexBufferMemory) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate vertex buffer memory!");
        }

        vkBindBufferMemory(device, m_BufferHandle, m_VertexBufferMemory, 0);

        void* data;
        vkMapMemory(device, m_VertexBufferMemory, 0, bufferInfo.size, 0, &data);
        memcpy(data, vertices.data(), (size_t)bufferInfo.size);
        vkUnmapMemory(device, m_VertexBufferMemory);
	}

	VulkanVertexBuffer::~VulkanVertexBuffer()
	{
		const VkDevice& device = VulkanRenderer::GetData().m_Device->device();

		vkDestroyBuffer(device, m_BufferHandle, nullptr);
		vkFreeMemory(device, m_VertexBufferMemory, nullptr);
	}
}

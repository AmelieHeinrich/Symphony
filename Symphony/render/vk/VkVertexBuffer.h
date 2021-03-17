#pragma once

#include <vulkan/vulkan.h>
#include "render/VertexBuffer.h"
#include <array>
#include <algorithm>

namespace symphony
{
	struct VulkanVertex : public Vertex
	{
        static VkVertexInputBindingDescription getBindingDescription() {
            VkVertexInputBindingDescription bindingDescription{};
            bindingDescription.binding = 0;
            bindingDescription.stride = sizeof(Vertex);
            bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

            return bindingDescription;
        }

        static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions() {
            std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};

            attributeDescriptions[0].binding = 0;
            attributeDescriptions[0].location = 0;
            attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
            attributeDescriptions[0].offset = offsetof(Vertex, Position);

            attributeDescriptions[1].binding = 0;
            attributeDescriptions[1].location = 1;
            attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
            attributeDescriptions[1].offset = offsetof(Vertex, Color);

            return attributeDescriptions;
        }
	};

	SymphonyClass VulkanVertexBuffer : public VertexBuffer
	{
	public:
		VulkanVertexBuffer(const std::vector<Vertex>& vertices);
		~VulkanVertexBuffer();

        virtual void* GetVertexBufferHandle() override {
            return (void*)m_BufferHandle;
        }
	private:
		VkBuffer m_BufferHandle;
		VkDeviceMemory m_VertexBufferMemory;
	};
}
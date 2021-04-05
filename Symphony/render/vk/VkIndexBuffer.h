#pragma once

#include "core/DLLExport.h"
#include <vulkan/vulkan.h>
#include "render/IndexBuffer.h"

namespace symphony
{
	SymphonyClass VulkanIndexBuffer : public IndexBuffer
	{
	public:
		VulkanIndexBuffer(const std::vector<uint32_t>& indices);
		virtual ~VulkanIndexBuffer();

		virtual void* GetIndexBufferHandle() override {
			return (void*)indexBuffer;
		}

		VkBuffer GetVulkanBuffer() {
			return indexBuffer;
		}
	private:
		VkBuffer indexBuffer;
		VkDeviceMemory indexBufferMemory;
	};
}
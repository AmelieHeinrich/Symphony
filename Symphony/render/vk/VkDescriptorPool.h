#pragma once

#include <vulkan/vulkan.h>

namespace symphony
{
	class DescriptorPool
	{
	public:
		DescriptorPool();
		~DescriptorPool();

		const VkDescriptorPool& GetDescriptorPool() const {
			return m_Pool;
		}
	private:
		VkDescriptorPool m_Pool;
	};
}
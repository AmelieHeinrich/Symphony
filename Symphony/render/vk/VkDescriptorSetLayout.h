#pragma once

#include <vulkan/vulkan.h>

namespace symphony
{
	class DescriptorSetLayout
	{
	public:
		DescriptorSetLayout();
		~DescriptorSetLayout();

		const VkDescriptorSetLayout& GetDescriptorSetLayout() const {
			return m_DescriptorSetLayout;
		}
	private:
		VkDescriptorSetLayout m_DescriptorSetLayout;
	};
}
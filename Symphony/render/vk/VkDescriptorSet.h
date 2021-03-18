#pragma once

#include <vulkan/vulkan.h>
#include <vector>

namespace symphony
{
	class DescriptorSet
	{
	public:
		DescriptorSet();
		~DescriptorSet();

		const std::vector<VkDescriptorSet>& GetDescriptorSet() const {
			return m_DescriptorSet;
		}
	private:
		std::vector<VkDescriptorSet> m_DescriptorSet;
	};
}
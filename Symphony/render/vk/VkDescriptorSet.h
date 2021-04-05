#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <memory>
#include "VkTexture2D.h"

namespace symphony
{
	class DescriptorSet
	{
	public:
		DescriptorSet(std::shared_ptr<VulkanTexture2D> t2d);
		~DescriptorSet();

		const std::vector<VkDescriptorSet>& GetDescriptorSet() const {
			return m_DescriptorSet;
		}
	private:
		std::vector<VkDescriptorSet> m_DescriptorSet;
	};
}
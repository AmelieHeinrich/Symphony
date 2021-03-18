#include "VkDescriptorSetLayout.h"
#include "VkRenderer.h"

namespace symphony
{
	DescriptorSetLayout::DescriptorSetLayout()
	{
		const VkDevice& device = VulkanRenderer::GetData().m_Device->device();

        VkDescriptorSetLayoutBinding uboLayoutBinding{};
        uboLayoutBinding.binding = 0;
        uboLayoutBinding.descriptorCount = 1;
        uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        uboLayoutBinding.pImmutableSamplers = nullptr;
        uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

        VkDescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = 1;
        layoutInfo.pBindings = &uboLayoutBinding;

        if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &m_DescriptorSetLayout) != VK_SUCCESS) {
            throw VulkanException("failed to create descriptor set layout!");
        }
	}

	DescriptorSetLayout::~DescriptorSetLayout()
	{
        const VkDevice& device = VulkanRenderer::GetData().m_Device->device();
        vkDestroyDescriptorSetLayout(device, m_DescriptorSetLayout, nullptr);
	}
}
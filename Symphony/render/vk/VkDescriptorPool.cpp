#include "VkDescriptorPool.h"
#include "VkRenderer.h"

namespace symphony
{
	DescriptorPool::DescriptorPool()
	{
        auto nrImages = VulkanRenderer::GetData().m_SwapChain->swap_chain_images().size();
        const VkDevice& device = VulkanRenderer::GetData().m_Device->device();

        VkDescriptorPoolSize poolSize{};
        poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSize.descriptorCount = static_cast<uint32_t>(nrImages);

        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = 1;
        poolInfo.pPoolSizes = &poolSize;
        poolInfo.maxSets = static_cast<uint32_t>(nrImages);

        if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &m_Pool) != VK_SUCCESS) {
            throw std::runtime_error("failed to create descriptor pool!");
        }
	}

	DescriptorPool::~DescriptorPool()
	{
        const VkDevice& device = VulkanRenderer::GetData().m_Device->device();
        vkDestroyDescriptorPool(device, m_Pool, nullptr);
	}
}

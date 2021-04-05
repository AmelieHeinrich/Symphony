#include "VkDescriptorPool.h"
#include "VkRenderer.h"

namespace symphony
{
	DescriptorPool::DescriptorPool()
	{
        auto nrImages = VulkanRenderer::GetData().m_SwapChain->swap_chain_images().size();
        const VkDevice& device = VulkanRenderer::GetData().m_Device->device();

        std::array<VkDescriptorPoolSize, 2> poolSizes{};
        poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSizes[0].descriptorCount = static_cast<uint32_t>(nrImages);
        poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        poolSizes[1].descriptorCount = static_cast<uint32_t>(nrImages);

        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
        poolInfo.pPoolSizes = poolSizes.data();
        poolInfo.maxSets = static_cast<uint32_t>(nrImages);

        if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &m_Pool) != VK_SUCCESS) {
            SY_CORE_ERROR("Vulkan: Failed to create descriptor pool!");
        }
	}

	DescriptorPool::~DescriptorPool()
	{
        const VkDevice& device = VulkanRenderer::GetData().m_Device->device();
        vkDestroyDescriptorPool(device, m_Pool, nullptr);
	}
}

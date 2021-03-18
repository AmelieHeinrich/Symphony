#include "VkDescriptorSet.h"
#include <vector>
#include "VkRenderer.h"
#include "render/Renderer.h"

namespace symphony
{
	DescriptorSet::DescriptorSet()
	{
        auto nrImages = VulkanRenderer::GetData().m_SwapChain->swap_chain_images().size();
        auto descriptorSetLayout = VulkanRenderer::GetData().descriptorSetLayout->GetDescriptorSetLayout();
        auto descriptorPool = VulkanRenderer::GetData().descriptorPool->GetDescriptorPool();
        auto device = VulkanRenderer::GetData().m_Device->device();

        std::vector<VkDescriptorSetLayout> layouts(nrImages, descriptorSetLayout);
        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = descriptorPool;
        allocInfo.descriptorSetCount = static_cast<uint32_t>(nrImages);
        allocInfo.pSetLayouts = layouts.data();

        m_DescriptorSet.resize(nrImages);
        if (vkAllocateDescriptorSets(device, &allocInfo, m_DescriptorSet.data()) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate descriptor sets!");
        }

        for (size_t i = 0; i < nrImages; i++) {
            VkDescriptorBufferInfo bufferInfo{};
            bufferInfo.buffer = VulkanRenderer::GetData().uniformBuffers[i];
            bufferInfo.offset = 0;
            bufferInfo.range = sizeof(RendererUniforms);

            VkWriteDescriptorSet descriptorWrite{};
            descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrite.dstSet = m_DescriptorSet[i];
            descriptorWrite.dstBinding = 0;
            descriptorWrite.dstArrayElement = 0;
            descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptorWrite.descriptorCount = 1;
            descriptorWrite.pBufferInfo = &bufferInfo;

            vkUpdateDescriptorSets(device, 1, &descriptorWrite, 0, nullptr);
        }
	}

	DescriptorSet::~DescriptorSet()
	{

	}
}

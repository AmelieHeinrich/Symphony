#include "VkDescriptorSet.h"
#include <vector>
#include "VkRenderer.h"
#include "VkTexture2D.h"

namespace symphony
{
	DescriptorSet::DescriptorSet()
	{
        auto nrImages = VulkanRenderer::GetData().m_SwapChain->swap_chain_images().size();
        auto descriptorSetLayout = VulkanRenderer::GetData().descriptorSetLayout->GetDescriptorSetLayout();
        auto descriptorPool = VulkanRenderer::GetData().descriptorPool->GetDescriptorPool();
        auto device = VulkanRenderer::GetData().m_Device->device();
        auto textures = VulkanRenderer::GetRendererTextures();

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
            bufferInfo.buffer = VulkanRenderer::GetData().uniformBuffers[i]->GetBuffer();
            bufferInfo.offset = 0;
            bufferInfo.range = sizeof(RendererUniforms);

            VkDescriptorImageInfo imageInfo{};
            imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            imageInfo.imageView = textures[0]->GetImageView();
            imageInfo.sampler = textures[0]->GetSampler();

            std::array<VkWriteDescriptorSet, 2> descriptorWrites{};

            descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[0].dstSet = m_DescriptorSet[i];
            descriptorWrites[0].dstBinding = 0;
            descriptorWrites[0].dstArrayElement = 0;
            descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptorWrites[0].descriptorCount = 1;
            descriptorWrites[0].pBufferInfo = &bufferInfo;

            descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[1].dstSet = m_DescriptorSet[i];
            descriptorWrites[1].dstBinding = 1;
            descriptorWrites[1].dstArrayElement = 0;
            descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            descriptorWrites[1].descriptorCount = 1;
            descriptorWrites[1].pImageInfo = &imageInfo;

            vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
        }
	}

	DescriptorSet::~DescriptorSet()
	{

	}
}

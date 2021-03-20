#pragma once

#include <vulkan/vulkan.h>
#include "VkShader.h"
#include "VkRenderPass.h"
#include "VkDescriptorSetLayout.h"
#include <memory>

namespace symphony
{
	struct GraphicsPipelineCreateInfo
	{
		std::shared_ptr<VulkanShader> PipelineShader;
		std::shared_ptr<RenderPass> PipelineRenderPass;
		std::shared_ptr<DescriptorSetLayout> PipelineDescriptorSetLayout;
		uint32_t Width;
		uint32_t Height;
		VkExtent2D SwapChainExtent;
	};

	class GraphicsPipeline
	{
	public:
		GraphicsPipeline(const VkDevice& device, GraphicsPipelineCreateInfo createInfo);
		~GraphicsPipeline();

		const VkPipeline& GetPipeline() const {
			return graphicsPipeline;
		}

		const VkPipelineLayout& GetPipelineLayout() const {
			return graphicsPipelineLayout;
		}

		const VkPipelineCache& GetPipelineCache() const {
			return graphicsPipelineCache;
		}
	private:
		VkPipeline graphicsPipeline;
		VkPipelineLayout graphicsPipelineLayout;
		VkPipelineCache graphicsPipelineCache;
		const VkDevice& deviceCopy;
	};
}
#pragma once

#include <vulkan/vulkan.h>
#include "VkShader.h"
#include "VkRenderPass.h"
#include <memory>

namespace symphony
{
	struct GraphicsPipelineCreateInfo
	{
		std::shared_ptr<VulkanShader> PipelineShader;
		std::shared_ptr<RenderPass> PipelineRenderPass;
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
	private:
		VkPipeline graphicsPipeline;
		VkPipelineLayout graphicsPipelineLayout;
		const VkDevice& deviceCopy;
	};
}
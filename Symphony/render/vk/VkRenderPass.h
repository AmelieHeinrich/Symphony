#pragma once

#include <vector>
#include <vulkan/vulkan.h>
#include <optional>

namespace symphony
{
	class RenderPass
	{
	public:
		RenderPass(const VkDevice& device, VkFormat swapchainformat);
		~RenderPass();

		const VkRenderPass& render_pass() const {
			return renderPass;
		}

		operator const VkRenderPass& () const {
			return renderPass;
		}
	private:
		const VkDevice& deviceCopy;
		VkRenderPass renderPass;
	};
}
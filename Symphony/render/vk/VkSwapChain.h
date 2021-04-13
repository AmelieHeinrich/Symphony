#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include "VkPhysicalDevice.h"

namespace symphony
{
	class SwapChain
	{
	public:
		SwapChain(VkPhysicalDevice gpu, VkSurfaceKHR surface, const VkDevice& device);
		~SwapChain();

		void InitFramebuffers(VkRenderPass renderPass);

		static VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
		static VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
		static VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

		const VkSwapchainKHR& swap_chain() const {
			return m_SwapChain;
		}

		const std::vector<VkImage>& swap_chain_images() const {
			return m_SwapChainImages;
		}

		const VkFormat& swap_chain_image_format() const {
			return m_SwapChainImageFormat;
		}

		const VkExtent2D& swap_chain_extent() const {
			return m_SwapChainExtent;
		}

		const std::vector<VkImageView>& swap_chain_image_views() const {
			return m_SwapChainImageViews;
		}

		const std::vector<VkFramebuffer>& swap_chain_framebuffers() const {
			return m_SwapChainFramebuffers;
		}

		operator const VkSwapchainKHR& () const {
			return m_SwapChain;
		}

		operator const VkExtent2D& () const {
			return m_SwapChainExtent;
		}

		operator const std::vector<VkImage>& () const {
			return  m_SwapChainImages;
		}

		operator const VkFormat& () const {
			return m_SwapChainImageFormat;
		}

		operator const std::vector<VkFramebuffer>& () const {
			return m_SwapChainFramebuffers;
		}
	private:
		const VkDevice& m_DeviceCopy;
		VkSwapchainKHR m_SwapChain;
		std::vector<VkImage> m_SwapChainImages;
		VkFormat m_SwapChainImageFormat;
		VkExtent2D m_SwapChainExtent;
		std::vector<VkImageView> m_SwapChainImageViews;
		std::vector<VkFramebuffer> m_SwapChainFramebuffers;
	};
}
#pragma once

#include "render/Texture.h"
#include <vulkan/vulkan.h>
#include "render/ImageData.h"
#include <vector>

namespace symphony
{
	class VulkanTexture2D : public Texture
	{
	public:
		VulkanTexture2D(const char* filepath);
		virtual ~VulkanTexture2D();

		virtual void* GetTextureBuffer() override {
			return (void*)m_TextureImage;
		}

		const VkImageView& GetImageView() const {
			return m_TextureImageView;
		}

		const VkSampler& GetSampler() const {
			return m_TextureSampler;
		}

		// A lot of vulkan texture utils
		static void CreateImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
		static void TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
		static void CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
		static VkImageView CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
		static VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
		static VkFormat FindDepthFormat();
		static bool HasStencilComponent(VkFormat format);
	private:
		VkImage m_TextureImage;
		VkDeviceMemory m_TextureImageMemory;
		VkImageView m_TextureImageView;
		VkSampler m_TextureSampler;
	};
}
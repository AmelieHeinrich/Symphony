#pragma once

#include <vulkan/vulkan.h>

#include "VkCommandBuffer.h"
#include "VkGraphicsPipeline.h"
#include <SDL.h>
#include "VkInstance.h"
#include "VkSurface.h"
#include "VkPhysicalDevice.h"
#include "VkDevice.h"
#include "VkSwapChain.h"
#include "VkRenderPass.h"
#include "VkCommandPool.h"
#include "VkShader.h"
#include "VkVertexBuffer.h"
#include "VkIndexBuffer.h"
#include "core/exception/VulkanException.h"
#include "VkDescriptorSetLayout.h"
#include "VkDescriptorPool.h"
#include "VkDescriptorSet.h"
#include <memory>
#include <vulkan/vulkan.h>
#include <vector>

#include "core/DLLExport.h"

namespace symphony
{
	class Window;

	struct RendererData
	{
		std::shared_ptr<Instance> m_Instance;
		std::shared_ptr<Surface> m_Surface;
		std::shared_ptr<PhysicalDevice> m_PhysicalDevice;
		std::shared_ptr<Device> m_Device;
		std::shared_ptr<SwapChain> m_SwapChain;
		std::shared_ptr<RenderPass> m_RenderPass;
		std::shared_ptr<CommandPool> m_CommandPool;
		std::vector<std::shared_ptr<CommandBuffer>> commandBuffers;
		std::shared_ptr<GraphicsPipeline> graphicsPipeline;

		std::shared_ptr<DescriptorSetLayout> descriptorSetLayout;
		std::shared_ptr<DescriptorPool> descriptorPool;
		std::shared_ptr<DescriptorSet> descriptorSet;

		std::vector<VkBuffer> uniformBuffers;
		std::vector<VkDeviceMemory> uniformBuffersMemory;

		std::vector<VkSemaphore> imageAvailableSemaphores;
		std::vector<VkSemaphore> renderFinishedSemaphores;
		std::vector<VkFence> inFlightFences;
		std::vector<VkFence> imagesInFlight;

		size_t currentFrame;
		float ClearColorR;
		float ClearColorG;
		float ClearColorB;
		float ClearColorA;
	};

	SymphonyClass VulkanRenderer
	{
	public:
		static void Init(Window* window);
		static void Prepare();
		static void Shutdown();

		static void ClearColor(float r, float g, float b, float a);
		static void Draw();

		static uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

		static void AddVertexBuffer(const std::vector<Vertex>& vertices);
		static void AddIndexBuffer(const std::vector<uint16_t>& indices);

		static const RendererData& GetData() {
			return s_Data;
		}
	private:
		static std::vector<std::shared_ptr<VulkanVertexBuffer>> m_VertexBuffers;
		static std::vector<std::shared_ptr<VulkanIndexBuffer>> m_IndexBuffers;
		static RendererData s_Data;
	};
}
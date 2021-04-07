#pragma once

#include <vulkan/vulkan.h>
#include <core/Log.h>
#include <unordered_map>

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
#include "VkUniformBuffer.h"
#include "VkTexture2D.h"
#include "VkMesh.h"
#include <memory>
#include <vulkan/vulkan.h>
#include <vector>
#include "render/Renderer.h"
#include "render/Mesh.h"

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
		std::shared_ptr<CommandBuffer> commandBuffer;
		std::shared_ptr<GraphicsPipeline> graphicsPipeline;

		std::shared_ptr<DescriptorSetLayout> descriptorSetLayout;
		std::shared_ptr<DescriptorPool> descriptorPool;

		VkImage DepthImage;
		VkDeviceMemory DepthImageMemory;
		VkImageView DepthImageView;

		std::vector<std::shared_ptr<VulkanUniformBuffer>> uniformBuffers;

		VkSemaphore imageAvailableSemaphores;
		VkSemaphore renderFinishedSemaphores;
		VkFence renderFence;

		uint32_t FBWidth;
		uint32_t FBHeight;

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

		static void PrintRendererInfo();
		static void AddVertexBuffer(const std::vector<Vertex>& vertices);
		static void AddIndexBuffer(const std::vector<uint32_t>& indices);
		static void AddTexture2D(const char* filepath);
		static void AddMesh(Mesh mesh, const std::string& name);
		static void SetMeshTransform(const std::string& meshName, const glm::mat4& transform);
		static void Resize(unsigned int width, unsigned int height);

		static const RendererData& GetData() {
			return s_Data;
		}
	private:
		static std::unordered_map<std::string, std::shared_ptr<VulkanMesh>> m_Meshes;
		static RendererData s_Data;
		static Window* targetWindow;
	};
}
#pragma once

#include "core/DLLExport.h"
#include "render/RenderSurface.h"
#include "VkCommandBuffer.h"
#include "VkGraphicsPipeline.h"
#include <memory>
#include <vulkan/vulkan.h>
#include <vector>

struct SDL_Window;

namespace symphony {

	class Instance;
	class PhysicalDevice;
	class Surface;
	class Device;
	class SwapChain;
	class RenderPass;
	class CommandPool;

	SymphonyClass VKRenderSurface : public RenderSurface
	{
	public:
		virtual void Init(void* extraparams) override;
		virtual void Shutdown() override;
		virtual void Clear() override;
		virtual void ClearColor(float r, float g, float b, float a) override;
		virtual void ResizeBuffers(int x, int y, int w, int h);
	private:
		SDL_Window* m_TargetWindow = nullptr;
		
		std::shared_ptr<Instance> m_Instance;
		std::shared_ptr<Surface> m_Surface;
		std::shared_ptr<PhysicalDevice> m_PhysicalDevice;
		std::shared_ptr<Device> m_Device;
		std::shared_ptr<SwapChain> m_SwapChain;
		std::shared_ptr<RenderPass> m_RenderPass;
		std::shared_ptr<CommandPool> m_CommandPool;
		std::vector<std::shared_ptr<CommandBuffer>> commandBuffers;
		std::shared_ptr<GraphicsPipeline> graphicsPipeline;

		std::vector<VkSemaphore> imageAvailableSemaphores;
		std::vector<VkSemaphore> renderFinishedSemaphores;
		std::vector<VkFence> inFlightFences;
		std::vector<VkFence> imagesInFlight;
		size_t currentFrame = 0;

		float ClearColorR = 0.0f;
		float ClearColorG = 0.0f;
		float ClearColorB = 0.0f;
		float ClearColorA = 0.0f;
	};
}
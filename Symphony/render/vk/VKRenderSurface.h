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
	};
}
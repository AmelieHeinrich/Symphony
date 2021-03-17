#include "VKRenderSurface.h"

#include <SDL.h>
#include "VkInstance.h"
#include "VkSurface.h"
#include "VkPhysicalDevice.h"
#include "VkDevice.h"
#include "VkSwapChain.h"
#include "VkRenderPass.h"
#include "VkCommandPool.h"
#include "VkShader.h"
#include "core/exception/VulkanException.h"

const int MAX_FRAMES_IN_FLIGHT = 2;

namespace symphony {

	void VKRenderSurface::Init(void* extraparams) {
		m_TargetWindow = (SDL_Window*)extraparams;
	}

	void VKRenderSurface::Shutdown() {

	}

	void VKRenderSurface::Clear() {
		
	}

	void VKRenderSurface::ClearColor(float r, float g, float b, float a) {
		
	}

	void VKRenderSurface::ResizeBuffers(int x, int y, int w, int h) {

	}
}
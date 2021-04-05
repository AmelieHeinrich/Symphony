#include "VkSurface.h"
#include "core/exception/VulkanException.h"
#include <core/Log.h>

namespace symphony
{
	Surface::Surface(VkInstance instance, SDL_Window* window)
		: instCopy(instance)
	{
		if (!SDL_Vulkan_CreateSurface(window, instance, &m_Surface)) {
			SY_CORE_ERROR("Vulkan: Failed to create window surface!");
		}
	}

	Surface::~Surface()
	{
		vkDestroySurfaceKHR(instCopy, m_Surface, nullptr);
	}
}
#pragma once

#include <vulkan/vulkan.h>
#include <SDL.h>
#include <SDL_vulkan.h>

namespace symphony
{
	class Surface
	{
	public:
		Surface(VkInstance instance, SDL_Window* window);
		~Surface();

		const VkSurfaceKHR& surface() const {
			return m_Surface;
		}

		operator const VkSurfaceKHR& () const {
			return m_Surface;
		}
	private:
		VkInstance instCopy;
		VkSurfaceKHR m_Surface;
	};
}
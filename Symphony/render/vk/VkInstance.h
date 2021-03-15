#pragma once

#include <vulkan/vulkan.h>
#include <SDL.h>

namespace symphony {
	class Instance
	{
	public:
		Instance(SDL_Window* window);
		~Instance();

		const VkInstance& instance() const {
			return m_Instance;
		}

		operator const VkInstance& () const {
			return m_Instance;
		}
	private:
		VkInstance m_Instance;
	};
}
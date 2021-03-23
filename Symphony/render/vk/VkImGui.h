#pragma once

#include "imgui.h"
#include "examples/imgui_impl_sdl.h"
#include "examples/imgui_impl_vulkan.h"
#include "SDL.h"

namespace symphony
{
	class Window;

	class VulkanImGui
	{
	public:
		static void Init(Window* window);
		static void Shutdown();

		static void NewFrame();
		static void EndFrame();

	private:
		static SDL_Window* targetWindow;
	};
}
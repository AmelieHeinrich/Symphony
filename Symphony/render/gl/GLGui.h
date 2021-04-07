#pragma once

#include <imgui.h>
#include <examples/imgui_impl_sdl.h>
#include <examples/imgui_impl_opengl3.h>
#include <SDL.h>

namespace symphony
{
	class GLGui
	{
	public:
		static void Init();
		static void Shutdown();
		static void BeginGUI();
		static void EndGUI();
	};
}
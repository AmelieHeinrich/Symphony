#pragma once

#include <imgui.h>
#include <examples/imgui_impl_dx11.h>
#include <examples/imgui_impl_sdl.h>

namespace symphony
{
	class DX11Gui
	{
	public:
		static void Init();
		static void Shutdown();
		static void BeginGUI();
		static void EndGUI();
	};
}
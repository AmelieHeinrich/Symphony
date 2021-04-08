#pragma once

#include <imgui.h>
#include <examples/imgui_impl_dx12.h>
#include <examples/imgui_impl_sdl.h>
#include <d3d12.h>
#include <memory>
#include "DX12Memory.h"

namespace symphony
{
	class DX12Gui
	{
	public:
		static void Init();
		static void Shutdown();
		static void BeginGUI();
		static void EndGUI();
	};
}

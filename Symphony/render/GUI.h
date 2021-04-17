#pragma once

#include "core/DLLExport.h"
#include <imgui.h>

namespace symphony
{
	SymphonyClass GUI
	{
	public:
		static void BeginGUI();
		static void EndGUI();

		static ImGuiContext* GetCurrentContext()
		{
			return ImGui::GetCurrentContext();
		}
	};
}
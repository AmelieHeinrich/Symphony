#pragma once

#include "core/DLLExport.h"

namespace symphony
{
	class GUI
	{
	public:
		static void Init();
		static void Shutdown();
		static void BeginGUI();
		static void EndGUI();
	};
}
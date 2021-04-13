#pragma once

#include <events/KeyCodes.h>
#include <events/MouseCodes.h>
#include <core/DLLExport.h>

namespace symphony
{
	SymphonyClass Input
	{
	public:
		static bool IsKeyPressed(KeyCode key);
		static bool IsKeyReleased(KeyCode key);
	};
}
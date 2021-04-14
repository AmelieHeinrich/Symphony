#pragma once

#include <SDL.h>
#include <core/DLLExport.h>

namespace symphony
{
	SymphonyClass InternalTimer
	{
	public:
		static void Init();
		static void Update();

		static float GetDeltaTime();
	private:
		static float Last;
		static float DeltaTime;
	};
}
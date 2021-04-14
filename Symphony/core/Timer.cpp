#include "Timer.h"

namespace symphony
{
	float InternalTimer::Last;
	float InternalTimer::DeltaTime;

	void InternalTimer::Init()
	{
		Last = 0.0f;
		DeltaTime = 0.0f;
	}

	void InternalTimer::Update()
	{
		float tick = (float)SDL_GetTicks();
		DeltaTime = (tick - Last) / 1000;
		Last = tick;
	}

	float InternalTimer::GetDeltaTime()
	{
		return DeltaTime;
	}
}

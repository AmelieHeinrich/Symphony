#pragma once

#include <cstdint>

namespace symphony
{
	using MouseCode = uint16_t;

	namespace Mouse
	{
		enum : MouseCode
		{
			// From SDL_mouse.h
			ButtonLeft = 1,
			ButtonMiddle = 2,
			ButtonRight = 3,
			ButtonX1 = 4,
			ButtonX2 = 5
		};
	}
}
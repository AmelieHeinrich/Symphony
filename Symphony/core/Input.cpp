#include "Input.h"
#include <SDL.h>
#include <core/Application.h>

namespace symphony
{
	bool Input::IsKeyPressed(KeyCode key)
	{
		const Uint8* keystate = SDL_GetKeyboardState(nullptr);

		if (keystate[key])
		{
			return true;
		}

		return false;
	}

	bool Input::IsKeyReleased(KeyCode key)
	{
		const Uint8* keystate = SDL_GetKeyboardState(nullptr);

		if (keystate[key])
		{
			return false;
		}

		return true;
	}
}

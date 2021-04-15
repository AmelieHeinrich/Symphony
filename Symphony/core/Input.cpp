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

	bool Input::IsMouseButtonPressed(MouseCode code)
	{
		int x;
		int y;
		Uint32 buttonstate = SDL_GetMouseState(&x, &y);
		Uint32 mask = 0;
		switch (code)
		{
		case Mouse::ButtonLeft:
			mask = SDL_BUTTON_LMASK;
			break;
		case Mouse::ButtonRight:
			mask = SDL_BUTTON_RMASK;
			break;
		case Mouse::ButtonMiddle:
			mask = SDL_BUTTON_MMASK;
			break;
		case Mouse::ButtonX1:
			mask = SDL_BUTTON_X1MASK;
			break;
		case Mouse::ButtonX2:
			mask = SDL_BUTTON_X2MASK;
			break;
		}
		
		return (mask & buttonstate);
	}

	bool Input::IsMouseButtonReleased(MouseCode code)
	{
		int x;
		int y;
		Uint32 buttonstate = SDL_GetMouseState(&x, &y);
		Uint32 mask = 0;
		switch (code)
		{
		case Mouse::ButtonLeft:
			mask = SDL_BUTTON_LMASK;
			break;
		case Mouse::ButtonRight:
			mask = SDL_BUTTON_RMASK;
			break;
		case Mouse::ButtonMiddle:
			mask = SDL_BUTTON_MMASK;
			break;
		case Mouse::ButtonX1:
			mask = SDL_BUTTON_X1MASK;
			break;
		case Mouse::ButtonX2:
			mask = SDL_BUTTON_X2MASK;
			break;
		}

		return !(mask & buttonstate);
	}

	int Input::GetMouseX()
	{
		int x;
		int y;
		SDL_GetMouseState(&x, &y);
		return x;
	}

	int Input::GetMouseY()
	{
		int x;
		int y;
		SDL_GetMouseState(&x, &y);
		return y;
	}
}

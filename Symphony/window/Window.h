#pragma once

struct SDL_Window;

#include "core/DLLExport.h"
#include "core/PlatformDetection.h"
#include "render/RenderSurface.h"

namespace symphony {

	SymphonyClass Window
	{
	public:
		Window(int width, int height, const char* title, RenderAPI api);
		~Window();

		bool IsWindowOpen();
		void Update();

		void Clear();
		void ClearColor(float r, float g, float b, float a);
		void Resize(int w, int h);

		static void InitialiseSDL2();
		static void TerminateSDL2();

		SDL_Window* GetWindowHandle() {
			return m_RawHandle;
		}
	private:
		RenderSurface* m_WindowSurface = nullptr;
		SDL_Window* m_RawHandle;
		bool m_IsWindowOpen = 1;
	};
};
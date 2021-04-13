#pragma once

struct SDL_Window;

#include "core/DLLExport.h"
#include "core/PlatformDetection.h"
#include "render/RenderSurface.h"
#include "events/Event.h"

namespace symphony {

	SymphonyClass Window
	{
	public:
		using EventCallbackFn = std::function<void(Event&)>;

		Window(int width, int height, const char* title, RenderAPI api);
		~Window();

		bool IsWindowOpen();
		void Update();

		void Clear();
		void ClearColor(float r, float g, float b, float a);
		void Resize(int w, int h);
		void LockMouse(bool lock);

		static void InitialiseSDL2();
		static void TerminateSDL2();

		SDL_Window* GetWindowHandle() {
			return m_RawHandle;
		}

		const RenderAPI& GetWindowRenderAPI() const {
			return m_API;
		}

		RenderSurface* GetRenderSurface() {
			return m_WindowSurface;
		}

		int GetWidth();
		int GetHeight();

		void SetEventCallback(const EventCallbackFn& callback) { m_EventCallback = callback; }
	private:
		RenderAPI m_API;
		RenderSurface* m_WindowSurface = nullptr;
		SDL_Window* m_RawHandle;
		bool m_IsWindowOpen = 1;

		EventCallbackFn m_EventCallback;
	};
};
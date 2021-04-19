#include "Window.h"
#include <SDL.h>
#include <glad/gl.h>
#include <iostream>

#include "render/dx11/DX11RenderSurface.h"
#include "render/gl/GLRenderSurface.h"
#include "render/vk/VKRenderSurface.h"
#include "render/dx12/DX12RenderSurface.h"
#include "render/vk/VkRenderer.h"
#include <imgui.h>
#include <examples/imgui_impl_sdl.h>

static bool GLFWInitialized = false;

namespace symphony {

	Window::Window(int width, int height, const char* title, RenderAPI api)
		: m_API(api)
	{	
		SDL_WindowFlags flag{};
	
		switch (api)
		{
		case RenderAPI::OpenGL:
			flag = SDL_WINDOW_OPENGL;
			m_WindowSurface = new GLRenderSurface();
			break;
		case RenderAPI::Vulkan:
			flag = SDL_WINDOW_VULKAN;
			m_WindowSurface = new VKRenderSurface();
			break;
		case RenderAPI::DirectX11:
			flag = SDL_WINDOW_SHOWN;
			m_WindowSurface = new DX11RenderSurface();
			break;
		case RenderAPI::DirectX12:
			flag = SDL_WINDOW_SHOWN;
			m_WindowSurface = new DX12RenderSurface();
			break;
		case RenderAPI::Metal:
			break;
		}

		m_RawHandle = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, flag);

		SDL_SetWindowResizable(m_RawHandle, SDL_TRUE);

		if (m_RawHandle == NULL) {
			__debugbreak();
		}

		m_WindowSurface->Init(m_RawHandle);
	}

	Window::~Window()
	{
		m_WindowSurface->Shutdown();
		delete m_WindowSurface;
		SDL_DestroyWindow(m_RawHandle);
		SDL_Quit();
	}

	bool Window::IsWindowOpen()
	{
		return m_IsWindowOpen;
	}

	void Window::Update()
	{
		m_WindowSurface->Clear();
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			ImGui_ImplSDL2_ProcessEvent(&event);

			switch (event.type)
			{
			case SDL_QUIT:
			{
				WindowCloseEvent wc_event;
				m_EventCallback(wc_event);
				m_IsWindowOpen = 0;
				break;
			}
			case SDL_WINDOWEVENT:
			{
				if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
					WindowResizeEvent wr_event(event.window.data1, event.window.data2);
					m_EventCallback(wr_event);
					Renderer::Resize(event.window.data1, event.window.data2);
				}
				break;
			}
			case SDL_KEYDOWN:
			{
				int repeat = event.key.repeat;

				KeyPressedEvent kp(event.key.keysym.scancode, repeat);
				m_EventCallback(kp);
				break;
			}
			case SDL_KEYUP:
			{
				KeyReleasedEvent kp(event.key.keysym.scancode);
				m_EventCallback(kp);
				break;
			}
			case SDL_MOUSEBUTTONDOWN:
			{
				MouseButtonPressedEvent mbp(event.button.button);
				m_EventCallback(mbp);
				break;
			}
			case SDL_MOUSEBUTTONUP:
			{
				MouseButtonReleasedEvent mbr(event.button.button);
				m_EventCallback(mbr);
				break;
			}
			case SDL_MOUSEMOTION:
			{
				MouseMovedEvent mme(event.motion.x, event.motion.y);
				m_EventCallback(mme);
				break;
			}
			case SDL_MOUSEWHEEL:
			{
				MouseScrolledEvent mse(event.wheel.x, event.wheel.y);
				m_EventCallback(mse);
				break;
			}
			}
		}
	}

	void Window::Clear()
	{
		m_WindowSurface->Clear();
	}
	
	void Window::ClearColor(float r, float g, float b, float a)
	{
		m_WindowSurface->ClearColor(r, g, b, a);
	}

	void Window::Resize(int w, int h)
	{
		SDL_SetWindowSize(m_RawHandle, w, h);
	}

	void Window::InitialiseSDL2()
	{
		SDL_Init(SDL_INIT_VIDEO);
	}

	void Window::TerminateSDL2()
	{
		SDL_Quit();
	}

	int Window::GetWidth()
	{
		int w;
		SDL_GetWindowSize(m_RawHandle, &w, nullptr);
		return w;
	}

	int Window::GetHeight()
	{
		int w;
		SDL_GetWindowSize(m_RawHandle, nullptr, &w);
		return w;
	}

	void Window::LockMouse(bool lock)
	{
		if (lock)
		{
			SDL_SetRelativeMouseMode(SDL_TRUE); SDL_ShowCursor(false);
			SDL_WarpMouseInWindow(m_RawHandle, GetWidth() / 2, GetHeight() / 2);
		}
		else
		{
			SDL_SetRelativeMouseMode(SDL_FALSE); SDL_ShowCursor(true);
		}
	}
}

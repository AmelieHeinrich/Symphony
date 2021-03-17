#include "Window.h"
#include <SDL.h>
#include <glad/gl.h>
#include <iostream>

#include "render/dx11/DX11RenderSurface.h"
#include "render/gl/GLRenderSurface.h"
#include "render/vk/VKRenderSurface.h"

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
		case RenderAPI::DirectX:
			flag = SDL_WINDOW_SHOWN;
			m_WindowSurface = new DX11RenderSurface();
			break;
		case RenderAPI::Metal:
			break;
		}

		m_RawHandle = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, flag);

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
			switch (event.type)
			{
			case SDL_QUIT:
				m_IsWindowOpen = 0;
				break;
			case SDL_WINDOWEVENT:
				if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
					m_WindowSurface->ResizeBuffers(0, 0, event.window.data1, event.window.data2);
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
		m_WindowSurface->ResizeBuffers(0, 0, w, h);
	}

	void Window::InitialiseSDL2()
	{
		SDL_Init(SDL_INIT_VIDEO);
	}

	void Window::TerminateSDL2()
	{
		
	}
}

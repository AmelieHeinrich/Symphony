#include "GLRenderSurface.h"
#include <iostream>
#include <glad/gl.h>

namespace symphony {

	void GLRenderSurface::Init(void* extraparams)
	{
		SDL_Window* window = (SDL_Window*)extraparams;
		m_TargetWindow = window;

		m_GLContext = SDL_GL_CreateContext(m_TargetWindow);
		gladLoadGL((GLADloadfunc)SDL_GL_GetProcAddress);
	}

	void GLRenderSurface::Clear()
	{
		SDL_GL_SwapWindow(m_TargetWindow);
		glEnable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void GLRenderSurface::ClearColor(float r, float g, float b, float a)
	{
		glClearColor(r, g, b, a);
	}

	void GLRenderSurface::Shutdown()
	{
		SDL_GL_DeleteContext(m_GLContext);
	}

	void GLRenderSurface::ResizeBuffers(int x, int y, int w, int h)
	{
		glViewport(x, y, w, h);
	}
}
#pragma once

#include "render/RenderSurface.h"
#include <SDL.h>

namespace symphony {
	SymphonyClass GLRenderSurface : public RenderSurface
	{
	public:
		void Init(void* extraparams) override;
		void Shutdown() override;
		void Clear() override;
		void ClearColor(float r, float g, float b, float a) override;
		void ResizeBuffers(int x, int y, int w, int h) override;
	private:
		SDL_Window* m_TargetWindow;
		SDL_GLContext m_GLContext;
	};
}
#pragma once

#include "render/RenderSurface.h"
#include "DX11SwapChain.h"
#include <SDL.h>
#include <d3d11.h>
#include <Windows.h>
#include "DX11RenderContext.h"

namespace symphony {
	SymphonyClass DX11RenderSurface : public RenderSurface
	{
	public:
		void Init(void* extraparams) override;
		void Shutdown() override;
		void Clear() override;
		void ClearColor(float r, float g, float b, float a) override;
		void ResizeBuffers(int x, int y, int w, int h) override;
	private:
		SDL_Window* m_TargetWindow;
	};
}

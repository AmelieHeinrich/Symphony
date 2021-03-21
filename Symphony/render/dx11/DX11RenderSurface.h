#pragma once

#include "render/RenderSurface.h"
#include "DX11SwapChain.h"
#include <SDL.h>
#include <d3d11.h>
#include <Windows.h>
#include "DX11RenderContext.h"

/*
	NOTE : The DX11 module is mostly inspired by the code I already written for a small renderer I made called Claire.
	I rewrote some stuff to fix big issues from the old code I wrote.
	What you are looking at is the "polished" version of the Claire renderer.
*/

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

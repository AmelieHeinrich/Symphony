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

		ID3D11Device* GetDevice() { return m_Device; }
		ID3D11DeviceContext* GetDeviceContext() { return m_Context;  }

		IDXGIDevice* GetDXGIDevice() { return m_DXGIDevice; }
		IDXGIAdapter* GetDXGIAdapter() { return m_DXGIAdapter; }
		IDXGIFactory* GetDXGIFactory() { return m_DXGIFactory; }
	private:
		SDL_Window* m_TargetWindow;
		HWND m_Win32Handle;

		ID3D11Device* m_Device;
		ID3D11DeviceContext* m_Context;
		D3D_FEATURE_LEVEL m_FeatureLevel;

		IDXGIDevice* m_DXGIDevice;
		IDXGIAdapter* m_DXGIAdapter;
		IDXGIFactory* m_DXGIFactory;

		DX11SwapChain* m_RendererSwapChain;
		DX11RenderContext* m_RendererContext;
	};
}

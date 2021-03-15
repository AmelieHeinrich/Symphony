#include "DX11RenderSurface.h"
#include <wrl.h>
#include <SDL_syswm.h>
#include <iostream>

namespace symphony {
	void DX11RenderSurface::Init(void* extraparams)
	{
		SDL_Window* window = (SDL_Window*)extraparams;
		m_TargetWindow = window;
	
		// Get Win32 HWND
		SDL_SysWMinfo wmInfo;
		SDL_VERSION(&wmInfo.version);
		SDL_GetWindowWMInfo(m_TargetWindow, &wmInfo);
		m_Win32Handle = wmInfo.info.win.window;

		D3D_DRIVER_TYPE driverTypes[] =
		{
			D3D_DRIVER_TYPE_HARDWARE,
			D3D_DRIVER_TYPE_WARP,
			D3D_DRIVER_TYPE_REFERENCE,
		};
		uint32_t driverSize = ARRAYSIZE(driverTypes);

		D3D_FEATURE_LEVEL levels[] =
		{
			D3D_FEATURE_LEVEL_11_0
		};
		uint32_t levelSize = ARRAYSIZE(levels);
		HRESULT success = 0;

		for (int i = 0; i < driverSize;)
		{
			success = D3D11CreateDevice(NULL, driverTypes[i], NULL, NULL, levels, levelSize, D3D11_SDK_VERSION, &m_Device, &m_FeatureLevel, &m_Context);

			if (SUCCEEDED(success))
				break;
			++i;
		}

		if (FAILED(success))
		{
			__debugbreak();
		}

		m_RendererContext = new DX11RenderContext(m_Context);

		m_Device->QueryInterface(__uuidof(IDXGIDevice), (void**)&m_DXGIDevice);
		m_DXGIDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&m_DXGIAdapter);
		m_DXGIAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&m_DXGIFactory);

		int w, h;
		SDL_GetWindowSize(m_TargetWindow, &w, &h);
		m_RendererSwapChain = new DX11SwapChain();
		m_RendererSwapChain->Create(this, m_Win32Handle, w, h);
	}

	void DX11RenderSurface::Shutdown()
	{
		m_RendererSwapChain->Release(this);
		m_DXGIDevice->Release();
		m_DXGIAdapter->Release();
		m_DXGIFactory->Release();

		m_RendererContext->Release();
		m_Device->Release();
	}

	void DX11RenderSurface::ClearColor(float r, float g, float b, float a)
	{
		m_RendererContext->SetClearColor(m_RendererSwapChain, r, g, b, a);
	}

	void DX11RenderSurface::Clear()
	{
		m_RendererSwapChain->Present(this);
		m_Context->ClearDepthStencilView(m_RendererSwapChain->GetDepthStencilView(), D3D11_CLEAR_DEPTH, 1, 0);
	}

	void DX11RenderSurface::ResizeBuffers(int x, int y, int w, int h)
	{
		m_RendererSwapChain->RecreateRenderTargetView(this, w, h);
	}
}
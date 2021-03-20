#pragma once

#include "core/DLLExport.h"
#include <d3d11.h>
#include <cstdint>

namespace symphony {

	class DX11RenderSurface;

	SymphonyClass DX11SwapChain
	{
	public:
		DX11SwapChain(HWND hwnd, uint32_t width, uint32_t height);
		~DX11SwapChain();

		void Present();
		void RecreateRenderTargetView(uint32_t width, uint32_t height);

		IDXGISwapChain* GetSwapChain() const { return m_Handle; }
		ID3D11RenderTargetView* GetRenderTargetView() const { return m_RenderTargetView; }
		ID3D11DepthStencilView* GetDepthStencilView() const { return m_DepthStencilView; }
		ID3D11Texture2D* GetDepthStencilBuffer() const { return m_DepthStencilBuffer; }
		ID3D11DepthStencilState* GetDepthStencilState() const { return m_DepthStencilState; }
		ID3D11RasterizerState* GetRasterizerState() const { return m_RasterizerState; }
	private:
		IDXGISwapChain* m_Handle;
		ID3D11RenderTargetView* m_RenderTargetView;
		ID3D11Texture2D* m_DepthStencilBuffer;
		ID3D11DepthStencilView* m_DepthStencilView;
		ID3D11DepthStencilState* m_DepthStencilState;
		ID3D11RasterizerState* m_RasterizerState;
		ID3D11SamplerState* m_SamplerState;

		friend class DX11RenderContext;
	};
}
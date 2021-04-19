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
		void EnsureSize(uint32_t width, uint32_t height);
		void Bind();
		void Unbind();

		IDXGISwapChain* GetSwapChain() const { return m_Handle; }
		ID3D11RenderTargetView* GetRenderTargetView() const { return m_RenderTargetView; }
		ID3D11Texture2D* GetRenderTargetTexture() const { return m_RenderTargetTexture; }
	private:
		void ReloadBuffers(uint32_t width, uint32_t height);

		uint32_t m_Width, m_Height;
		IDXGISwapChain* m_Handle;
		ID3D11RenderTargetView* m_RenderTargetView;
		ID3D11Texture2D* m_RenderTargetTexture;

		friend class DX11RenderContext;
	};
}
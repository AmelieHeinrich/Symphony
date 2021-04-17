#include "DX11SwapChain.h"
#include "DX11RenderSurface.h"
#include "DX11Renderer.h"

#include <wrl.h>
#include <SDL_syswm.h>
#include "core/Application.h"

namespace symphony {
	DX11SwapChain::DX11SwapChain(HWND hwnd, uint32_t width, uint32_t height)
		: m_Width(width), m_Height(height)
	{
		auto device = DX11Renderer::GetRendererData().Device;
		auto factory = DX11Renderer::GetRendererData().DXGIFactory;

		DXGI_SWAP_CHAIN_DESC swapDesc{};
		swapDesc.BufferDesc.Width = m_Width;
		swapDesc.BufferDesc.Height = m_Height;
		swapDesc.BufferDesc.RefreshRate.Numerator = 60;
		swapDesc.BufferDesc.RefreshRate.Denominator = 1;
		swapDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		swapDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		swapDesc.SampleDesc.Count = 1;		// Not used in D3D12
		swapDesc.SampleDesc.Quality = 0;	// Not used in D3D12
		swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapDesc.BufferCount = 1;
		swapDesc.OutputWindow = hwnd;
		swapDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
		swapDesc.Windowed = TRUE;

		HRESULT result = DX11Renderer::GetRendererData().DXGIFactory->CreateSwapChain(device, &swapDesc, &m_Handle);

		DX11Renderer::CheckIfFailed(result, "D3D11: Failed to create Swap chain object!");

		ReloadBuffers(m_Width, m_Height);

		result = factory->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER);
		DX11Renderer::CheckIfFailed(result, "D3D11: Failed to make window assocation with swap chain!");
	}

	void DX11SwapChain::ReloadBuffers(uint32_t width, uint32_t height)
	{
		auto device = DX11Renderer::GetRendererData().Device;
		m_Handle->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&m_RenderTargetTexture);
		device->CreateRenderTargetView(m_RenderTargetTexture, nullptr, &m_RenderTargetView);
	}

	DX11SwapChain::~DX11SwapChain()
	{
		m_RenderTargetTexture->Release();
		m_RenderTargetView->Release();
		m_Handle->Release();
	}

	void DX11SwapChain::Present()
	{
		m_Handle->Present(0, 0);
	}

	void DX11SwapChain::RecreateRenderTargetView(uint32_t width, uint32_t height)
	{
		m_Width = width;
		m_Height = height;

		m_RenderTargetTexture->Release();
		m_RenderTargetView->Release();
		m_Handle->ResizeBuffers(1, m_Width, m_Height, DXGI_FORMAT_R8G8B8A8_UNORM, 0);
		ReloadBuffers(m_Width, m_Height);
	}

	void DX11SwapChain::EnsureSize(uint32_t width, uint32_t height)
	{
		if (m_Width != width || m_Height != height)
			RecreateRenderTargetView(width, height);
	}
}
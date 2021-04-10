#include "DX11SwapChain.h"
#include "DX11RenderSurface.h"
#include "DX11Renderer.h"

#include <wrl.h>
#include <SDL_syswm.h>
#include "core/Application.h"

namespace symphony {
	DX11SwapChain::DX11SwapChain(HWND hwnd, uint32_t width, uint32_t height)
	{
		auto device = DX11Renderer::GetRendererData().Device;
		auto factory = DX11Renderer::GetRendererData().DXGIFactory;

		DXGI_SWAP_CHAIN_DESC swapChainInfo;
		ZeroMemory(&swapChainInfo, sizeof(swapChainInfo));
		swapChainInfo.BufferCount = 1;
		swapChainInfo.BufferDesc.Width = width;
		swapChainInfo.BufferDesc.Height = height;
		swapChainInfo.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainInfo.BufferDesc.RefreshRate.Numerator = 256; // TODO: Select refresh rate
		swapChainInfo.BufferDesc.RefreshRate.Denominator = 1;
		swapChainInfo.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainInfo.OutputWindow = hwnd;
		swapChainInfo.SampleDesc.Count = 1;
		swapChainInfo.SampleDesc.Quality = 0;
		swapChainInfo.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
		swapChainInfo.Windowed = TRUE;

		HRESULT result = DX11Renderer::GetRendererData().DXGIFactory->CreateSwapChain(device, &swapChainInfo, &m_Handle);

		DX11Renderer::CheckIfFailed(result, "D3D11: Failed to create Swap chain object!");

		ReloadBuffers(width, height);

		result = factory->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER);
		DX11Renderer::CheckIfFailed(result, "D3D11: Failed to make window assocation with swap chain!");
	}

	void DX11SwapChain::ReloadBuffers(uint32_t width, uint32_t height)
	{
		auto device = DX11Renderer::GetRendererData().Device;

		ID3D11Texture2D* buffer = NULL;
		HRESULT result = m_Handle->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&buffer);

		DX11Renderer::CheckIfFailed(result, "D3D11: Failed to create swap chain texture object!");

		result = device->CreateRenderTargetView(buffer, NULL, &m_RenderTargetView);
		buffer->Release();

		DX11Renderer::CheckIfFailed(result, "D3D11: Failed to create swap chain render target view object!");

		D3D11_TEXTURE2D_DESC depthStencilDesc;
		depthStencilDesc.Width = width;
		depthStencilDesc.Height = height;
		depthStencilDesc.MipLevels = 1;
		depthStencilDesc.ArraySize = 1;
		depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilDesc.SampleDesc.Count = 1;
		depthStencilDesc.SampleDesc.Quality = 0;
		depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
		depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthStencilDesc.CPUAccessFlags = 0;
		depthStencilDesc.MiscFlags = 0;

		result = device->CreateTexture2D(&depthStencilDesc, NULL, &m_DepthStencilBuffer);
		DX11Renderer::CheckIfFailed(result, "D3D11: Failed to create swap chain texture object!");

		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {};
		depthStencilViewDesc.Format = depthStencilDesc.Format;
		if (depthStencilDesc.SampleDesc.Count > 1) depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
		else depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depthStencilViewDesc.Texture2D.MipSlice = 0;

		result = device->CreateDepthStencilView(m_DepthStencilBuffer, &depthStencilViewDesc, &m_DepthStencilView);
		DX11Renderer::CheckIfFailed(result, "D3D11: Failed to create depth stencil view!");

		D3D11_DEPTH_STENCIL_DESC depthstencildesc = {};
		depthstencildesc.DepthEnable = true;
		depthstencildesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ALL;
		depthstencildesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS;

		result = device->CreateDepthStencilState(&depthstencildesc, &m_DepthStencilState);
		DX11Renderer::CheckIfFailed(result, "D3D11: Failed to depth stencil state!");

		D3D11_RASTERIZER_DESC rasterizerDesc = {};
		rasterizerDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
		rasterizerDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_NONE;
		rasterizerDesc.FrontCounterClockwise = true;
		result = device->CreateRasterizerState(&rasterizerDesc, &m_RasterizerState);
		DX11Renderer::CheckIfFailed(result, "D3D11: Failed to create rasterizer state!");

		D3D11_SAMPLER_DESC sampDesc = {};
		sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		sampDesc.MinLOD = 0;
		sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
		result = device->CreateSamplerState(&sampDesc, &m_SamplerState); //Create sampler state
		DX11Renderer::CheckIfFailed(result, "D3D11: Failed to create sampler state!");

		DX11Renderer::GetRendererData().Context->OMSetRenderTargets(1, &m_RenderTargetView, m_DepthStencilView);
		DX11Renderer::GetRendererData().Context->OMSetDepthStencilState(m_DepthStencilState, 0);

		D3D11_VIEWPORT vp;
		vp.Width = (FLOAT)width;
		vp.Height = (FLOAT)height;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;
		DX11Renderer::GetRendererData().Context->RSSetViewports(1, &vp);
	}

	DX11SwapChain::~DX11SwapChain()
	{
		m_SamplerState->Release();
		m_RasterizerState->Release();
		m_DepthStencilState->Release();
		m_DepthStencilView->Release();
		m_DepthStencilBuffer->Release();
		m_RenderTargetView->Release();
		m_Handle->Release();
	}

	void DX11SwapChain::Present()
	{
		m_Handle->Present(0, 0);
	}

	void DX11SwapChain::RecreateRenderTargetView(uint32_t width, uint32_t height)
	{
		if (m_Handle)
		{
			m_SamplerState->Release();
			m_RasterizerState->Release();
			m_DepthStencilState->Release();
			m_DepthStencilView->Release();
			m_DepthStencilBuffer->Release();
			m_RenderTargetView->Release();
			m_Handle->ResizeBuffers(1, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, 0);
			ReloadBuffers(width, height);
		}
	}
}
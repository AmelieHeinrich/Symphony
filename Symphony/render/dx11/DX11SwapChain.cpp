#include "DX11SwapChain.h"
#include "DX11RenderSurface.h"

namespace symphony {
	void DX11SwapChain::Create(DX11RenderSurface* surface, HWND hwnd, uint32_t width, uint32_t height)
	{
		auto device = surface->GetDevice();

		DXGI_SWAP_CHAIN_DESC swapChainInfo;
		ZeroMemory(&swapChainInfo, sizeof(swapChainInfo));
		swapChainInfo.BufferCount = 1;
		swapChainInfo.BufferDesc.Width = width;
		swapChainInfo.BufferDesc.Height = height;
		swapChainInfo.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainInfo.BufferDesc.RefreshRate.Numerator = 60; // TODO: Select refresh rate
		swapChainInfo.BufferDesc.RefreshRate.Denominator = 1;
		swapChainInfo.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainInfo.OutputWindow = hwnd;
		swapChainInfo.SampleDesc.Count = 1;
		swapChainInfo.SampleDesc.Quality = 0;
		swapChainInfo.Windowed = TRUE;

		HRESULT result = surface->GetDXGIFactory()->CreateSwapChain(device, &swapChainInfo, &m_Handle);

		if (FAILED(result))
		{
			__debugbreak();
		}

		ID3D11Texture2D* buffer = NULL;
		result = m_Handle->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&buffer);

		if (FAILED(result))
		{
			__debugbreak();
		}

		result = device->CreateRenderTargetView(buffer, NULL, &m_RenderTargetView);
		buffer->Release();

		if (FAILED(result))
		{
			__debugbreak();
		}

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
		if (FAILED(result))
		{
			__debugbreak();
		}

		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {};
		depthStencilViewDesc.Format = depthStencilDesc.Format;
		if (depthStencilDesc.SampleDesc.Count > 1) depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
		else depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depthStencilViewDesc.Texture2D.MipSlice = 0;

		result = device->CreateDepthStencilView(m_DepthStencilBuffer, &depthStencilViewDesc, &m_DepthStencilView);
		if (FAILED(result))
		{
			__debugbreak();
		}

		D3D11_DEPTH_STENCIL_DESC depthstencildesc = {};
		depthstencildesc.DepthEnable = true;
		depthstencildesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ALL;
		depthstencildesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;

		result = device->CreateDepthStencilState(&depthstencildesc, &m_DepthStencilState);
		if (FAILED(result))
		{
			__debugbreak();
		}

		D3D11_RASTERIZER_DESC rasterizerDesc = {};
		rasterizerDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
		rasterizerDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;
		result = device->CreateRasterizerState(&rasterizerDesc, &m_RasterizerState);
		if (FAILED(result))
		{
			__debugbreak();
		}

		D3D11_SAMPLER_DESC sampDesc = {};
		sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		sampDesc.MinLOD = 0;
		sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
		result = device->CreateSamplerState(&sampDesc, &m_SamplerState); //Create sampler state
		if (FAILED(result))
		{
			__debugbreak();
		}

		surface->GetDeviceContext()->OMSetRenderTargets(1, &m_RenderTargetView, m_DepthStencilView);
		surface->GetDeviceContext()->OMSetDepthStencilState(m_DepthStencilState, 0);

		D3D11_VIEWPORT vp;
		vp.Width = (FLOAT)width;
		vp.Height = (FLOAT)height;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;
		surface->GetDeviceContext()->RSSetViewports(1, &vp);
	}

	void DX11SwapChain::Release(DX11RenderSurface* surface)
	{
		m_SamplerState->Release();
		m_RasterizerState->Release();
		m_DepthStencilState->Release();
		m_DepthStencilView->Release();
		m_DepthStencilBuffer->Release();
		m_RenderTargetView->Release();
		m_Handle->Release();
		delete this;
	}

	void DX11SwapChain::Present(DX11RenderSurface* surface)
	{
		m_Handle->Present(1, 0);
	}

	void DX11SwapChain::RecreateRenderTargetView(DX11RenderSurface* surface, uint32_t width, uint32_t height)
	{
		if (m_Handle != nullptr)
		{
			surface->GetDeviceContext()->OMSetRenderTargets(0, 0, 0);
			m_RenderTargetView->Release();

			HRESULT hr;
			hr = m_Handle->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);

			if (FAILED(hr))
				__debugbreak();

			ID3D11Texture2D* buffer = NULL;
			hr = m_Handle->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&buffer);

			if (FAILED(hr))
			{
				__debugbreak();
			}

			hr = surface->GetDevice()->CreateRenderTargetView(buffer, NULL, &m_RenderTargetView);
			buffer->Release();

			surface->GetDeviceContext()->OMSetRenderTargets(1, &m_RenderTargetView, m_DepthStencilView);

			D3D11_VIEWPORT vp;
			vp.Width = width;
			vp.Height = height;
			vp.MinDepth = 0.0f;
			vp.MaxDepth = 1.0f;
			vp.TopLeftX = 0;
			vp.TopLeftY = 0;
			surface->GetDeviceContext()->RSSetViewports(1, &vp);
		}
	}
}
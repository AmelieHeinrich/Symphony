#include "DX11RenderTexture.h"
#include "DX11Renderer.h"

namespace symphony
{
	DX11RenderTexture::DX11RenderTexture(uint32_t width, uint32_t height)
		: m_Width(width), m_Height(height)
	{
		LoadBuffers(width, height);

		D3D11_VIEWPORT vp;
		vp.Width = (FLOAT)width;
		vp.Height = (FLOAT)height;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;
		DX11Renderer::GetRendererData().Context->RSSetViewports(1, &vp);
	}

	DX11RenderTexture::~DX11RenderTexture()
	{
		m_DepthStencilState->Release();
		m_DepthStencilView->Release();
		m_DepthStencilBuffer->Release();
		m_RenderTargetView->Release();
		m_Texture->Release();
	}

	void DX11RenderTexture::Clear(const glm::vec4& color)
	{
		auto ctx = DX11Renderer::GetRendererData().Context;

		float ClearColorValues[] = { color.x, color.y, color.z, color.w };
		ctx->ClearRenderTargetView(m_RenderTargetView, ClearColorValues);
		ctx->ClearDepthStencilView(m_DepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	}

	void DX11RenderTexture::EnsureSize(uint32_t width, uint32_t height)
	{
		DX11Renderer::GetRendererData().RendererSwapChain->EnsureSize(width, height);

		if (m_Width != width || m_Height != height)
			Resize(width, height);
	}

	void DX11RenderTexture::Resize(uint32_t width, uint32_t height)
	{
		m_Width = width;
		m_Height = height;

		m_DepthStencilState->Release();
		m_DepthStencilView->Release();
		m_DepthStencilBuffer->Release();
		m_RenderTargetView->Release();

		auto device = DX11Renderer::GetRendererData().Device;
		auto ctx = DX11Renderer::GetRendererData().Context;

		ctx->OMSetRenderTargets(0, 0, 0);

		LoadBuffers(width, height);

		Bind();
	}

	void DX11RenderTexture::Bind()
	{
		auto ctx = DX11Renderer::GetRendererData().Context;
		ctx->OMSetDepthStencilState(m_DepthStencilState, 0);
		ctx->OMSetRenderTargets(1, &m_RenderTargetView, m_DepthStencilView);
	}

	void DX11RenderTexture::Unbind()
	{

	}

	void DX11RenderTexture::LoadBuffers(uint32_t width, uint32_t height)
	{
		auto device = DX11Renderer::GetRendererData().Device;

		HRESULT res;

		res = DX11Renderer::GetRendererData().RendererSwapChain->GetSwapChain()->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&m_Texture);
		DX11Renderer::CheckIfFailed(res, "D3D11: Failed to get swap chain buffer!");

		res = device->CreateRenderTargetView(m_Texture, nullptr, &m_RenderTargetView);
		DX11Renderer::CheckIfFailed(res, "D3D11: Failed to create rtv!");

		D3D11_TEXTURE2D_DESC textureDesc;
		textureDesc.Width = m_Width;
		textureDesc.Height = m_Height;
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 1;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = 0;
		textureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		textureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {};
		depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depthStencilViewDesc.Texture2D.MipSlice = 0;

		res = device->CreateTexture2D(&textureDesc, nullptr, &m_DepthStencilBuffer);
		DX11Renderer::CheckIfFailed(res, "D3D11: Failed to create depth stencil buffer!");
		res = device->CreateDepthStencilView(m_DepthStencilBuffer, &depthStencilViewDesc, &m_DepthStencilView);

		D3D11_DEPTH_STENCIL_DESC depthstencildesc = {};
		depthstencildesc.DepthEnable = true;
		depthstencildesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ALL;
		depthstencildesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS;
		device->CreateDepthStencilState(&depthstencildesc, &m_DepthStencilState);
	}
}
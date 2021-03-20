#include "DX11RenderContext.h"
#include "DX11RenderSurface.h"

namespace symphony {
	DX11RenderContext::DX11RenderContext(ID3D11DeviceContext* context)
		: m_DeviceContext(context)
	{
		m_DeviceContext->Release();
	}

	DX11RenderContext::~DX11RenderContext()
	{
	}

	void DX11RenderContext::SetClearColor(std::shared_ptr<DX11SwapChain> swapChain, float r, float g, float b, float a)
	{
		float ClearColorValues[] = { r, g, b, a };
		m_DeviceContext->OMSetDepthStencilState(swapChain->GetDepthStencilState(), 0);
		m_DeviceContext->OMSetRenderTargets(1, &swapChain->m_RenderTargetView, swapChain->m_DepthStencilView);
		m_DeviceContext->ClearRenderTargetView(swapChain->GetRenderTargetView(), ClearColorValues);
		m_DeviceContext->ClearDepthStencilView(swapChain->GetDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		m_DeviceContext->RSSetState(swapChain->m_RasterizerState);
	}
}
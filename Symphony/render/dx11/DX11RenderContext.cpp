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
		
	}
}
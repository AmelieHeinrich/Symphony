#pragma once

#include <d3d11.h>
#include "DX11SwapChain.h"

namespace symphony {
	class DX11RenderSurface;
	class DX11RenderContext
	{
	public:
		DX11RenderContext(ID3D11DeviceContext* context);

		void SetClearColor(DX11SwapChain* swapChain, float r, float g, float b, float a);

		void Release();

		ID3D11DeviceContext* GetDeviceContext() const { return m_DeviceContext; }
	private:
		ID3D11DeviceContext* m_DeviceContext;
		friend class DX11SwapChain;

	};
}
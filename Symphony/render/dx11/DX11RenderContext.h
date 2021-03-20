#pragma once

#include <d3d11.h>
#include "DX11SwapChain.h"
#include <memory>

namespace symphony {

	class DX11RenderContext
	{
	public:
		DX11RenderContext(ID3D11DeviceContext* context);
		~DX11RenderContext();

		void SetClearColor(std::shared_ptr<DX11SwapChain> swapChain, float r, float g, float b, float a);

		ID3D11DeviceContext* GetDeviceContext() const { return m_DeviceContext; }
	private:
		ID3D11DeviceContext* m_DeviceContext;
		friend class DX11SwapChain;
	};
}
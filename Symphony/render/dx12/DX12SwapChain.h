#pragma once

#include <d3d12.h>
#include <dxgi1_4.h>
#include <vector>
#include "window/Window.h"

namespace symphony
{
	class DX12SwapChain
	{
	public:
		DX12SwapChain(Window* window);
		
		void ReleaseBackBuffers();
		void ReleaseSwapChain();

		void Present();
		void Resize(uint32_t width, uint32_t height);

		IDXGISwapChain3* GetSwapChain() {
			return swapChain;
		}

		const std::vector<ID3D12Resource*>& GetBackBuffers() const {
			return backBuffers;
		}
	private:
		IDXGISwapChain3* swapChain;
		std::vector<ID3D12Resource*> backBuffers;
	};
}
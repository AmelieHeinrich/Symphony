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
		
		void ReleaseSwapChain();

		void Present();
		void Resize(uint32_t width, uint32_t height);
		void CreateDepthBuffer();

		IDXGISwapChain3* GetSwapChain() {
			return swapChain;
		}

		const std::vector<ID3D12Resource*>& GetBackBuffers() const {
			return backBuffers;
		}
	private:
		ID3D12Resource* DepthResource;
		IDXGISwapChain3* swapChain;
		std::vector<ID3D12Resource*> backBuffers;
	};
}
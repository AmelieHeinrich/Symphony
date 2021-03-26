#include "DX12SwapChain.h"
#include "DX12Renderer.h"

#include <wrl.h>
#include <SDL_syswm.h>

namespace symphony
{
	DX12SwapChain::DX12SwapChain(Window* window)
	{
		auto device = DX12Renderer::GetRendererData().RendererDevice->GetDevice();
		auto factory = DX12Renderer::GetRendererData().RendererDevice->GetFactory();
		auto commandQueue = DX12Renderer::GetRendererData().RendererCommand->GetCommandQueue();
		auto memory = DX12Renderer::GetRendererData().RendererMemory->GetHeapHandle();

		SDL_SysWMinfo wmInfo;
		SDL_VERSION(&wmInfo.version);
		SDL_GetWindowWMInfo(window->GetWindowHandle(), &wmInfo);
		HWND windowRaw = wmInfo.info.win.window;

		int w, h;
		SDL_GetWindowSize(window->GetWindowHandle(), &w, &h);

		DXGI_SWAP_CHAIN_DESC swapDesc;
		swapDesc.BufferDesc.Width = w;
		swapDesc.BufferDesc.Height = h;
		swapDesc.BufferDesc.RefreshRate.Numerator = 60;
		swapDesc.BufferDesc.RefreshRate.Denominator = 1;
		swapDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		swapDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		swapDesc.SampleDesc.Count = 1;		// Not used in D3D12
		swapDesc.SampleDesc.Quality = 0;	// Not used in D3D12
		swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapDesc.BufferCount = 2;
		swapDesc.OutputWindow = windowRaw;
		swapDesc.Windowed = TRUE;
		swapDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

		DX12Device::ThrowIfFailed(factory->CreateSwapChain(commandQueue, &swapDesc, &swapChain));
		factory->MakeWindowAssociation(windowRaw, DXGI_MWA_NO_ALT_ENTER);

		UINT uiDescHeapSizeRTV = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

		backBuffers.resize(2);
		for (int i = 0; i < 2; i++) {
			backBuffers[i] = nullptr;

			swapChain->GetBuffer(i, IID_PPV_ARGS(&backBuffers[i]));
			device->CreateRenderTargetView(backBuffers[i], NULL, memory);

			memory.ptr += uiDescHeapSizeRTV;
		}
	}

	void DX12SwapChain::ReleaseBackBuffers()
	{
		for (auto i : backBuffers)
			i->Release();
		backBuffers.clear();
	}

	void DX12SwapChain::ReleaseSwapChain()
	{
		
	}

	void DX12SwapChain::Present()
	{
		DX12Device::ThrowIfFailed(swapChain->Present(0, 0));
	}
}
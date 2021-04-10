#include "DX12SwapChain.h"
#include "DX12Renderer.h"

#include <wrl.h>
#include <SDL_syswm.h>
#include "core/Application.h"
#include <examples/imgui_impl_dx12.h>

namespace symphony
{
	DX12SwapChain::DX12SwapChain(Window* window)
	{
		auto device = DX12Renderer::GetRendererData().RendererDevice->GetDevice();
		auto factory = DX12Renderer::GetRendererData().RendererDevice->GetFactory();
		auto commandQueue = DX12Renderer::GetRendererData().CommandQueue;
		auto memory = DX12HeapManager::RenderTargetViewHeap->GetHeapHandle();

		SDL_SysWMinfo wmInfo;
		SDL_VERSION(&wmInfo.version);
		SDL_GetWindowWMInfo(window->GetWindowHandle(), &wmInfo);
		HWND windowRaw = wmInfo.info.win.window;

		int w, h;
		SDL_GetWindowSize(window->GetWindowHandle(), &w, &h);

		IDXGISwapChain* temp;
		DXGI_SWAP_CHAIN_DESC swapDesc{};
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

		auto res = factory->CreateSwapChain(commandQueue, &swapDesc, &temp);
		swapChain = static_cast<IDXGISwapChain3*>(temp);
		DX12Renderer::CheckIfFailed(res, "D3D12: Failed to create swap chain!");

		res = factory->MakeWindowAssociation(windowRaw, DXGI_MWA_NO_ALT_ENTER);
		DX12Renderer::CheckIfFailed(res, "D3D12: Failed to make window assocation with swap chain!");

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
		auto res = swapChain->Present(0, 0);
		DX12Renderer::CheckIfFailed(res, "D3D12: Failed to present swap chain!");
	}

	void DX12SwapChain::Resize(uint32_t width, uint32_t height)
	{
		if (swapChain)
		{
			DX12Renderer::GetCurrentCommand()->SignalFence(DX12Renderer::GetRendererData().RendererFences[DX12Renderer::GetRendererData().BufferIndex]);
			DX12Renderer::GetRendererData().RendererFences[DX12Renderer::GetRendererData().BufferIndex]->WaitEvents();

			ImGui_ImplDX12_InvalidateDeviceObjects();

			auto device = DX12Renderer::GetRendererData().RendererDevice->GetDevice();
			auto factory = DX12Renderer::GetRendererData().RendererDevice->GetFactory();
			auto commandQueue = DX12Renderer::GetRendererData().CommandQueue;
			auto memory = DX12HeapManager::RenderTargetViewHeap->GetHeapHandle();

			SDL_Window* window = Application::Get().GetWindow().GetWindowHandle();

			SDL_SysWMinfo wmInfo;
			SDL_VERSION(&wmInfo.version);
			SDL_GetWindowWMInfo(window, &wmInfo);
			HWND windowRaw = wmInfo.info.win.window;

			DXGI_SWAP_CHAIN_DESC desc = {};
			swapChain->GetDesc(&desc);
			desc.BufferDesc.Width = width;
			desc.BufferDesc.Height = height;
			swapChain->ResizeBuffers(1, width, height, desc.BufferDesc.Format, desc.Flags);

			UINT uiDescHeapSizeRTV = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

			factory->MakeWindowAssociation(windowRaw, DXGI_MWA_NO_ALT_ENTER);

			for (int i = 0; i < 2; i++) {
				backBuffers[i]->Release();
				backBuffers[i] = nullptr;

				swapChain->GetBuffer(i, IID_PPV_ARGS(&backBuffers[i]));
				device->CreateRenderTargetView(backBuffers[i], NULL, memory);

				memory.ptr += uiDescHeapSizeRTV;
			}

			ImGui_ImplDX12_CreateDeviceObjects();

			D3D12_VIEWPORT view{};
			view.Width = width;
			view.Height = height;
			view.MaxDepth = 1.0f;
			view.MinDepth = 0.0f;

			D3D12_RECT scissor{ 0 };
			scissor.right = view.Width;
			scissor.bottom = view.Height;

			DX12Renderer::GetCurrentCommand()->GetCommandList()->RSSetViewports(1, &view);
			DX12Renderer::GetCurrentCommand()->GetCommandList()->RSSetScissorRects(1, &scissor);
		}
	}
}
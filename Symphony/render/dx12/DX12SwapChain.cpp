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
			swapChain->GetBuffer(i, IID_PPV_ARGS(&backBuffers[i]));
			device->CreateRenderTargetView(backBuffers[i], NULL, memory);

			memory.ptr += uiDescHeapSizeRTV;
		}
	}

	void DX12SwapChain::ReleaseSwapChain()
	{
		DepthResource->Release();
		for (int i = 0; i < 2; i++)
		{
			backBuffers[i]->Release();
		}
		swapChain->Release();
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

			auto device = DX12Renderer::GetRendererData().RendererDevice->GetDevice();
			
			for (int i = 0; i < 2; i++)
			{
				backBuffers[i]->Release();
			}
			DepthResource->Release();

			swapChain->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0);
			
			auto memory = DX12HeapManager::RenderTargetViewHeap->GetHeapHandle();
			UINT uiDescHeapSizeRTV = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

			for (int i = 0; i < 2; i++) {
				swapChain->GetBuffer(i, IID_PPV_ARGS(&backBuffers[i]));

				device->CreateRenderTargetView(backBuffers[i], NULL, memory);

				memory.ptr += uiDescHeapSizeRTV;
			}

			CreateDepthBuffer();
		}
	}

	void DX12SwapChain::CreateDepthBuffer()
	{
		D3D12_HEAP_PROPERTIES dsHeapProperties;
		ZeroMemory(&dsHeapProperties, sizeof(&dsHeapProperties));
		dsHeapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
		dsHeapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		dsHeapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		dsHeapProperties.CreationNodeMask = NULL;
		dsHeapProperties.VisibleNodeMask = NULL;

		D3D12_RESOURCE_DESC dsResDesc;
		ZeroMemory(&dsResDesc, sizeof(D3D12_RESOURCE_DESC));
		dsResDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		dsResDesc.Alignment = 0;
		dsResDesc.Width = DX12Renderer::GetRendererData().FBWidth;
		dsResDesc.Height = DX12Renderer::GetRendererData().FBHeight;
		dsResDesc.DepthOrArraySize = 1;
		dsResDesc.MipLevels = 1;
		dsResDesc.Format = DXGI_FORMAT_D32_FLOAT;
		dsResDesc.SampleDesc.Count = 1;
		dsResDesc.SampleDesc.Quality = 0;
		dsResDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		dsResDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

		D3D12_CLEAR_VALUE clearValueDs = {};
		ZeroMemory(&clearValueDs, sizeof(D3D12_CLEAR_VALUE));
		clearValueDs.Format = DXGI_FORMAT_D32_FLOAT;
		clearValueDs.DepthStencil.Depth = 1.0f;
		clearValueDs.DepthStencil.Stencil = 0;

		auto res = DX12Renderer::GetRendererData().RendererDevice->GetDevice()->CreateCommittedResource(
			&dsHeapProperties,
			D3D12_HEAP_FLAG_NONE,
			&dsResDesc,
			D3D12_RESOURCE_STATE_DEPTH_WRITE,
			&clearValueDs,
			IID_PPV_ARGS(&DepthResource)
		);
		DX12Renderer::CheckIfFailed(res, "D3D12: Failed to create depth stencil view!");

		D3D12_DEPTH_STENCIL_VIEW_DESC dsViewDesk = {};
		ZeroMemory(&dsViewDesk, sizeof(D3D12_DEPTH_STENCIL_VIEW_DESC));
		dsViewDesk.Format = DXGI_FORMAT_D32_FLOAT;
		dsViewDesk.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		dsViewDesk.Flags = D3D12_DSV_FLAG_NONE;
		dsViewDesk.Texture2D.MipSlice = 0;

		D3D12_CPU_DESCRIPTOR_HANDLE heapHandleDsv = DX12HeapManager::DepthResourceHeap->GetHeapHandle();
		DX12Renderer::GetRendererData().RendererDevice->GetDevice()->CreateDepthStencilView(DepthResource, &dsViewDesk, heapHandleDsv);
	}
}
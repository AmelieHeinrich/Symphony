#include "DX12Command.h"
#include "DX12Renderer.h"

namespace symphony
{
	DX12Command::DX12Command()
	{
		auto device = DX12Renderer::GetRendererData().RendererDevice->GetDevice();

		D3D12_COMMAND_QUEUE_DESC queDesc;
		queDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		queDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
		queDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		queDesc.NodeMask = NULL;

		DX12Device::ThrowIfFailed(device->CreateCommandQueue(&queDesc, IID_PPV_ARGS(&commandQueue)));
		DX12Device::ThrowIfFailed(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator)));
		DX12Device::ThrowIfFailed(device->CreateCommandList(NULL, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator, NULL, IID_PPV_ARGS(&commandList)));

		uiDescHeapSizeRTV = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	}

	DX12Command::~DX12Command()
	{
		commandList->Release();
		commandAllocator->Release();
		commandQueue->Release();
	}

	void DX12Command::CloseCommandList()
	{
		commandList->Close();
	}

	void DX12Command::ExecuteCommandList()
	{
		ID3D12CommandList* arr[] = {
			commandList
		};

		commandQueue->ExecuteCommandLists(1, arr);
	}

	void DX12Command::SignalFence(std::shared_ptr<DX12Fence> fence)
	{
		commandQueue->Signal(fence->GetFence(), fence->GetUIFence());
	}

	void DX12Command::ResetCommandAllocatorAndList()
	{
		commandAllocator->Reset();
		commandList->Reset(commandAllocator, NULL);
	}

	void DX12Command::BeginFrame(uint32_t bufferIndex)
	{
		auto ptrsBackBuffers = DX12Renderer::GetRendererData().RendererSwapChain->GetBackBuffers();

		D3D12_RESOURCE_BARRIER baPresentToRtv;
		baPresentToRtv.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		baPresentToRtv.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		baPresentToRtv.Transition.pResource = ptrsBackBuffers[bufferIndex];
		baPresentToRtv.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		baPresentToRtv.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
		baPresentToRtv.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
		commandList->ResourceBarrier(1, &baPresentToRtv);
	}

	void DX12Command::EndFrame(uint32_t bufferIndex)
	{
		auto ptrsBackBuffers = DX12Renderer::GetRendererData().RendererSwapChain->GetBackBuffers();

		D3D12_RESOURCE_BARRIER baRtvToPresent;
		baRtvToPresent.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		baRtvToPresent.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		baRtvToPresent.Transition.pResource = ptrsBackBuffers[bufferIndex];
		baRtvToPresent.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		baRtvToPresent.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		baRtvToPresent.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
		commandList->ResourceBarrier(1, &baRtvToPresent);
	}

	void DX12Command::Clear(uint32_t bufferIndex)
	{
		auto device = DX12Renderer::GetRendererData().RendererDevice->GetDevice();
		auto memory = DX12Renderer::GetRendererData().RendererMemory->GetHeapHandle();
		memory.ptr += (SIZE_T)uiDescHeapSizeRTV * bufferIndex;

		static FLOAT clearColorValues[4] = { ccr, ccg, ccb, cca };
		commandList->ClearRenderTargetView(memory, clearColorValues, 0, NULL);
		commandList->OMSetRenderTargets(1, &memory, TRUE, NULL);
	}

	void DX12Command::ClearColor(float r, float g, float b, float a)
	{
		ccr = r;
		ccg = g;
		ccb = b;
		cca = a;
	}
}
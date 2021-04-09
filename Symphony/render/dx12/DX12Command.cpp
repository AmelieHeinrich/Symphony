#include "DX12Command.h"
#include "DX12Renderer.h"
#include "dx12ext/d3dx12.h"

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

		auto res = device->CreateCommandQueue(&queDesc, IID_PPV_ARGS(&commandQueue));
		DX12Renderer::CheckIfFailed(res, "D3D12: Failed to create command queue!");
		res = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator));
		DX12Renderer::CheckIfFailed(res, "D3D12: Failed to create command allocator!");
		res = device->CreateCommandList(NULL, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator, NULL, IID_PPV_ARGS(&commandList));
		DX12Renderer::CheckIfFailed(res, "D3D12: Failed to create command list!");

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
		commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(ptrsBackBuffers[bufferIndex], D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));
	}

	void DX12Command::EndFrame(uint32_t bufferIndex)
	{
		auto ptrsBackBuffers = DX12Renderer::GetRendererData().RendererSwapChain->GetBackBuffers();

		commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(ptrsBackBuffers[bufferIndex], D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));
	}

	void DX12Command::Clear(uint32_t bufferIndex)
	{
		auto device = DX12Renderer::GetRendererData().RendererDevice->GetDevice();
		auto memory = DX12HeapManager::RenderTargetViewHeap->GetHeapHandle();
		auto depthMemory = DX12HeapManager::DepthResourceHeap->GetHeapHandle();
		memory.ptr += (SIZE_T)uiDescHeapSizeRTV * bufferIndex;

		static FLOAT clearColorValues[4] = { ccr, ccg, ccb, cca };
		commandList->OMSetRenderTargets(1, &memory, TRUE, &depthMemory);
		commandList->ClearRenderTargetView(memory, clearColorValues, 0, NULL);
		commandList->ClearDepthStencilView(depthMemory, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, NULL);
	}

	void DX12Command::ClearColor(float r, float g, float b, float a)
	{
		ccr = r;
		ccg = g;
		ccb = b;
		cca = a;
	}
}
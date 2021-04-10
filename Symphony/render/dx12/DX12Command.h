#pragma once

#include <d3d12.h>
#include "DX12Fence.h"
#include <cstdint>
#include <memory>

namespace symphony
{
	class DX12Command
	{
	public:
		DX12Command();
		~DX12Command();

		void CloseCommandList();
		void ExecuteCommandList();
		void ResetCommandAllocator();
		void SignalFence(std::shared_ptr<DX12Fence> fence);
		void ResetCommandAllocatorAndList();
		void BeginFrame(uint32_t bufferIndex);
		void EndFrame(uint32_t bufferIndex);
		void Clear(uint32_t bufferIndex);
		void ClearColor(float r, float g, float b, float a);

		ID3D12CommandAllocator* GetCommandAllocator() {
			return commandAllocator;
		}

		ID3D12GraphicsCommandList* GetCommandList() {
			return commandList;
		}
	private:
		ID3D12CommandAllocator* commandAllocator = nullptr;
		ID3D12GraphicsCommandList* commandList = nullptr;
		UINT uiDescHeapSizeRTV;
		float ccr = 0.0f;
		float ccg = 0.0f;
		float ccb = 0.0f;
		float cca = 0.0f;
	};
}
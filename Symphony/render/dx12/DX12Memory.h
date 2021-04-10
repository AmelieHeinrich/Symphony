#pragma once

#include <d3d12.h>
#include <cstdint>

namespace symphony
{
	class DX12Memory
	{
	public:
		DX12Memory(D3D12_DESCRIPTOR_HEAP_TYPE memoryType, D3D12_DESCRIPTOR_HEAP_FLAGS flags, int numDescriptors);
		~DX12Memory();

		ID3D12DescriptorHeap* GetDescriptorHeap() {
			return descriptorHeap;
		}

		D3D12_CPU_DESCRIPTOR_HANDLE& GetHeapHandle();
		D3D12_GPU_DESCRIPTOR_HANDLE& GetGPUHandle();
	private:
		ID3D12DescriptorHeap* descriptorHeap = nullptr;
		D3D12_DESCRIPTOR_HEAP_TYPE m_Type;
	};
}
#pragma once

#include <d3d12.h>

namespace symphony
{
	class DX12Memory
	{
	public:
		DX12Memory();
		~DX12Memory();

		ID3D12DescriptorHeap* GetDescriptorHeap() {
			return descriptorHeap;
		}

		D3D12_CPU_DESCRIPTOR_HANDLE GetHeapHandle();
	private:
		ID3D12DescriptorHeap* descriptorHeap = nullptr;
	};
}
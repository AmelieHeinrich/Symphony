#include "DX12Memory.h"
#include "DX12Renderer.h"

namespace symphony
{
	DX12Memory::DX12Memory(D3D12_DESCRIPTOR_HEAP_TYPE memoryType, D3D12_DESCRIPTOR_HEAP_FLAGS flags, int numDescriptors)
	{
		auto device = DX12Renderer::GetRendererData().RendererDevice->GetDevice();

		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
		rtvHeapDesc.Type = memoryType;
		rtvHeapDesc.NumDescriptors = numDescriptors;
		rtvHeapDesc.Flags = flags;
		rtvHeapDesc.NodeMask = NULL;

		DX12Device::ThrowIfFailed(device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&descriptorHeap)));
	}

	DX12Memory::~DX12Memory()
	{
		descriptorHeap->Release();
	}

	D3D12_CPU_DESCRIPTOR_HANDLE DX12Memory::GetHeapHandle()
	{
		return descriptorHeap->GetCPUDescriptorHandleForHeapStart();
	}

	D3D12_GPU_DESCRIPTOR_HANDLE DX12Memory::GetGPUHandle()
	{
		return descriptorHeap->GetGPUDescriptorHandleForHeapStart();
	}
}
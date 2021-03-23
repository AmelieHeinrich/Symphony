#include "DX12Memory.h"
#include "DX12Renderer.h"

namespace symphony
{
	DX12Memory::DX12Memory()
	{
		auto device = DX12Renderer::GetRendererData().RendererDevice->GetDevice();

		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
		rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtvHeapDesc.NumDescriptors = 2;
		rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
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
}
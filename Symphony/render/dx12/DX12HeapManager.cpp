#include "DX12HeapManager.h"
#include "DX12Renderer.h"

namespace symphony
{
	std::shared_ptr<DX12Memory> DX12HeapManager::RenderTargetViewHeap;
	std::shared_ptr<DX12Memory> DX12HeapManager::DepthResourceHeap;
	std::shared_ptr<DX12Memory> DX12HeapManager::ShaderHeap;

	void DX12HeapManager::Init()
	{
		RenderTargetViewHeap = std::make_shared<DX12Memory>(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, 2);
		DepthResourceHeap = std::make_shared<DX12Memory>(D3D12_DESCRIPTOR_HEAP_TYPE_DSV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, 1);
	}

	void DX12HeapManager::Release()
	{
		ShaderHeap.reset();
		DepthResourceHeap.reset();
		ShaderHeap.reset();
	}

	void DX12HeapManager::InitMeshHeap()
	{
		ShaderHeap = std::make_shared<DX12Memory>(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, DX12Renderer::GetNumMeshes());
	}
}
#include "DX12UniformBuffer.h"
#include "DX12Device.h"
#include "DX12Renderer.h"
#include <glm/glm.hpp>

namespace symphony
{
	DX12UniformBuffer::DX12UniformBuffer()
	{
		auto device = DX12Renderer::GetRendererData().RendererDevice->GetDevice();
		auto mainDescriptorHeap = DX12HeapManager::RenderTargetViewHeap->GetHeapHandle();

		D3D12_HEAP_PROPERTIES props{};
		props.Type = D3D12_HEAP_TYPE_UPLOAD;
		props.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		props.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		props.CreationNodeMask = 1;
		props.VisibleNodeMask = 1;

		D3D12_RESOURCE_DESC resourceDesc;
		ZeroMemory(&resourceDesc, sizeof(resourceDesc));
		resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		resourceDesc.Alignment = 0;
		resourceDesc.Width = sizeof(RendererUniforms);
		resourceDesc.Height = 1;
		resourceDesc.DepthOrArraySize = 1;
		resourceDesc.MipLevels = 1;
		resourceDesc.Format = DXGI_FORMAT_UNKNOWN;
		resourceDesc.SampleDesc.Count = 1;
		resourceDesc.SampleDesc.Quality = 0;
		resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

		auto res = device->CreateCommittedResource(&props, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_Resource));
		DX12Renderer::CheckIfFailed(res, "D3D12: Failed to create ubo!");

		m_View = {};

		m_View.BufferLocation = m_Resource->GetGPUVirtualAddress();
		m_View.SizeInBytes = 256;
		device->CreateConstantBufferView(&m_View, DX12HeapManager::ConstantBufferHeap->GetHeapHandle());
	}

	DX12UniformBuffer::~DX12UniformBuffer()
	{
		m_Resource->Release();
	}

	void DX12UniformBuffer::Bind(uint32_t offset)
	{
		auto frameIndex = DX12Renderer::GetRendererData().BufferIndex;
		auto clist = DX12Renderer::GetRendererData().RendererCommand->GetCommandList();
		clist->SetGraphicsRootConstantBufferView(0, m_Resource->GetGPUVirtualAddress() + offset);
	}

	void DX12UniformBuffer::Unbind()
	{
		//auto frameIndex = DX12Renderer::GetRendererData().BufferIndex;
		//auto clist = DX12Renderer::GetRendererData().RendererCommand->GetCommandList();
		//clist->SetGraphicsRootConstantBufferView(0, 0);
	}

	void DX12UniformBuffer::Update(RendererUniforms uniforms)
	{
		void* data;
		m_Resource->Map(0, nullptr, reinterpret_cast<void**>(&data));
		memcpy(data, &uniforms, sizeof(uniforms));
		m_Resource->Unmap(0, nullptr);
	}
}
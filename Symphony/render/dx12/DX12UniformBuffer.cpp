#include "DX12UniformBuffer.h"
#include "DX12Device.h"
#include "DX12Renderer.h"
#include <glm/glm.hpp>

namespace symphony
{
	DX12UniformBuffer::DX12UniformBuffer()
	{
		auto device = DX12Renderer::GetRendererData().RendererDevice->GetDevice();
		auto mainDescriptorHeap = DX12Renderer::GetRendererData().RendererMemory->GetHeapHandle();

		m_DescriptorHeap = std::make_shared<DX12Memory>(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, 1);

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

		DX12Device::ThrowIfFailed(device->CreateCommittedResource(&props, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_Resource)));

		D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
		cbvDesc.BufferLocation = m_Resource->GetGPUVirtualAddress();
		cbvDesc.SizeInBytes = 256;
		device->CreateConstantBufferView(&cbvDesc, m_DescriptorHeap->GetHeapHandle());
	}

	DX12UniformBuffer::~DX12UniformBuffer()
	{
		m_Resource->Release();
		m_DescriptorHeap.reset();
	}

	void DX12UniformBuffer::Bind()
	{
		auto clist = DX12Renderer::GetRendererData().RendererCommand->GetCommandList();
		ID3D12DescriptorHeap* descriptorHeaps[] = { m_DescriptorHeap->GetDescriptorHeap() };
		clist->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
		clist->SetGraphicsRootDescriptorTable(0, m_DescriptorHeap->GetGPUHandle());
	}

	void DX12UniformBuffer::Unbind()
	{

	}

	void DX12UniformBuffer::Update(RendererUniforms uniforms)
	{
		void* data;
		m_Resource->Map(0, nullptr, reinterpret_cast<void**>(&data));
		memcpy(data, &uniforms, sizeof(uniforms));
		m_Resource->Unmap(0, nullptr);
	}
}
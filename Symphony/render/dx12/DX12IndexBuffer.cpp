#include "DX12IndexBuffer.h"
#include "DX12VertexBuffer.h"
#include "DX12Renderer.h"

namespace symphony
{
	DX12IndexBuffer::DX12IndexBuffer(const std::vector<uint32_t>& indices)
		: IndexBuffer(indices)
	{
		m_Handle = CreateDefaultBuffer<uint32_t>(indices, D3D12_RESOURCE_STATE_INDEX_BUFFER);

		D3D12_INDEX_BUFFER_VIEW controlPointsBufferView;
		controlPointsBufferView.BufferLocation = m_Handle->GetGPUVirtualAddress();
		controlPointsBufferView.Format = DXGI_FORMAT_R32_UINT;
		controlPointsBufferView.SizeInBytes = indices.size() * sizeof(uint32_t);

		m_View = controlPointsBufferView;
	}

	DX12IndexBuffer::~DX12IndexBuffer()
	{
		m_Handle->Release();
	}

	void DX12IndexBuffer::Bind() const
	{
		auto clist = DX12Renderer::GetRendererData().RendererCommand->GetCommandList();
		clist->IASetIndexBuffer(&m_View);
	}

	void DX12IndexBuffer::Unbind() const
	{
		auto clist = DX12Renderer::GetRendererData().RendererCommand->GetCommandList();
		clist->IASetIndexBuffer(nullptr);
	}
}
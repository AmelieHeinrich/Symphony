#include "DX12VertexBuffer.h"
#include "DX12Renderer.h"
#include "DX12Device.h"
#include <wrl.h>

namespace symphony
{
	DX12VertexBuffer::DX12VertexBuffer(const std::vector<Vertex>& vertices)
		: VertexBuffer(vertices)
	{
		m_Resource = CreateDefaultBuffer<Vertex>(vertices, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);

		D3D12_VERTEX_BUFFER_VIEW controlPointsBufferView;
		controlPointsBufferView.BufferLocation = m_Resource->GetGPUVirtualAddress();
		controlPointsBufferView.StrideInBytes = static_cast<UINT>(sizeof(Vertex));
		controlPointsBufferView.SizeInBytes = static_cast<UINT>(controlPointsBufferView.StrideInBytes * vertices.size());

		m_VertexBufferView = controlPointsBufferView;
	}

	DX12VertexBuffer::~DX12VertexBuffer()
	{
		m_Resource->Release();
	}

	void DX12VertexBuffer::Bind()
	{
		auto clist = DX12Renderer::GetRendererData().RendererCommand->GetCommandList();
		clist->IASetVertexBuffers(0, 1, &m_VertexBufferView);
	}

	void DX12VertexBuffer::Unbind()
	{
		auto clist = DX12Renderer::GetRendererData().RendererCommand->GetCommandList();
		clist->IASetVertexBuffers(0, 1, nullptr);
	}
}
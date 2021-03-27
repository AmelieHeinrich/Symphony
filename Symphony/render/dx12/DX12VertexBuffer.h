#pragma once

#include "render/VertexBuffer.h"
#include <d3d12.h>

namespace symphony
{
	class DX12VertexBuffer : public VertexBuffer
	{
	public:
		DX12VertexBuffer(const std::vector<Vertex>& vertices);
		virtual ~DX12VertexBuffer();

		void Bind();
		void Unbind();

		static ID3D12Resource* CreateDefaultBuffer(const std::vector<Vertex>& data, D3D12_RESOURCE_STATES finalState);

		virtual void* GetVertexBufferHandle() override {
			return (void*)&m_VertexBufferView;
		}

		virtual ID3D12Resource* GetVertexBuffer() {
			return m_Resource;
		}
	private:
		ID3D12Resource* m_Resource;
		D3D12_VERTEX_BUFFER_VIEW m_VertexBufferView;
	private:
		friend class RenderContext;
	};
}
#pragma once

#include "render/VertexBuffer.h"
#include <d3d11.h>

namespace symphony
{
	class DX11VertexBuffer : public VertexBuffer
	{
	public:
		DX11VertexBuffer(const std::vector<Vertex>& vertices);
		virtual ~DX11VertexBuffer();

		void Bind();
		void Unbind();

		virtual void* GetVertexBufferHandle() override {
			return (void*)m_Buffer;
		}

	private:
		void* GetPointerInternal();
		ID3D11Buffer* m_Buffer;
		D3D11_BUFFER_DESC m_BufferDesc;
		D3D11_MAPPED_SUBRESOURCE m_MappedSubresource;
		ID3D11InputLayout* m_InputLayout;
		std::vector<D3D11_INPUT_ELEMENT_DESC> m_InputElementDescription;
	private:
		friend class RenderContext;
	};
}
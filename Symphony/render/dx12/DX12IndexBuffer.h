#pragma once

#include "render/IndexBuffer.h"
#include <d3d12.h>

namespace symphony
{
	class DX12IndexBuffer : public IndexBuffer
	{
	public:
		DX12IndexBuffer(const std::vector<uint32_t>& indices);
		~DX12IndexBuffer();

		void Bind() const;
		void Unbind() const;

		virtual void* GetIndexBufferHandle() {
			return (void*)&m_View;
		}
	private:
		ID3D12Resource* m_Handle;
		D3D12_INDEX_BUFFER_VIEW m_View;
	};
}
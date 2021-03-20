#pragma once

#include <d3d11.h>
#include "render/IndexBuffer.h"

namespace symphony
{
	class DX11IndexBuffer : public IndexBuffer
	{
	public:
		DX11IndexBuffer(const std::vector<uint16_t>& indices);
		~DX11IndexBuffer();

		void Bind() const;
		void Unbind() const;

		virtual void* GetIndexBufferHandle() {
			return (void*)m_Handle;
		}
	private:
		ID3D11Buffer* m_Handle;
	};
}
#pragma once

#include <d3d11.h>
#include <cstdint>
#include "render/Renderer.h"

namespace symphony
{
	class DX11UniformBuffer
	{
	public:
		DX11UniformBuffer(void* data, uint32_t size);
		~DX11UniformBuffer();

		void BindForShader(int bufferIndex);
		void Unbind(int bufferIndex);

		void Update(void* data);

		ID3D11Buffer* GetBufferHandle() const { return m_BufferHandle; }
	private:
		ID3D11Buffer* m_BufferHandle = nullptr;
	};
}
#pragma once

#include <d3d11.h>
#include <cstdint>
#include "render/Renderer.h"

namespace symphony
{
	class DX11UniformBuffer
	{
	public:
		DX11UniformBuffer();
		~DX11UniformBuffer();

		void BindForShader(int bufferIndex);
		void Unbind();

		void Update(RendererUniforms ubo);

		ID3D11Buffer* GetBufferHandle() const { return m_BufferHandle; }
	private:
		ID3D11Buffer* m_BufferHandle = nullptr;
	};
}
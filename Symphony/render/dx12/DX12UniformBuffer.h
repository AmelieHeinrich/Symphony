#pragma once

#include "DX12Memory.h"
#include "render/Renderer.h"
#include <d3d12.h>
#include <memory>

namespace symphony
{
	class DX12UniformBuffer
	{
	public:
		DX12UniformBuffer();
		~DX12UniformBuffer();

		void Bind(uint32_t offset = 0);
		void Unbind();
		void Update(RendererUniforms uniforms);
	private:
		ID3D12Resource* m_Resource;
		D3D12_CONSTANT_BUFFER_VIEW_DESC m_View;
	};
}
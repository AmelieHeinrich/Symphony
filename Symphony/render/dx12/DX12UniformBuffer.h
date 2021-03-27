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

		void Bind();
		void Unbind();
		void Update(RendererUniforms uniforms);

		std::shared_ptr<DX12Memory> GetDescriptorHeap() {
			return m_DescriptorHeap;
		}
	private:
		std::shared_ptr<DX12Memory> m_DescriptorHeap;
		ID3D12Resource* m_Resource;
		D3D12_CONSTANT_BUFFER_VIEW_DESC m_View;
	};
}
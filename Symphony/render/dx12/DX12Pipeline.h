#pragma once

#include <d3d12.h>
#include "DX12Shader.h"
#include <memory>

namespace symphony
{
	struct DX12PipelineCreateInfo
	{
		std::shared_ptr<DX12Shader> PipelineShader;
		bool Multisampled;
		uint32_t MultisampleCount;
	};

	class DX12Pipeline
	{
	public:
		DX12Pipeline(DX12PipelineCreateInfo pci);
		~DX12Pipeline();

		void Bind();
		void Unbind();
	private:
		ID3D12PipelineState* m_PSO = nullptr;
	};
}
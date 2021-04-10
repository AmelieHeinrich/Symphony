#include "DX12Pipeline.h"
#include "DX12Device.h"
#include "DX12Renderer.h"

namespace symphony
{
	DX12Pipeline::DX12Pipeline(DX12PipelineCreateInfo pci)
	{
		auto device = DX12Renderer::GetRendererData().RendererDevice->GetDevice();

		D3D12_DEPTH_STENCIL_DESC depthStencilDesc;
		ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
		depthStencilDesc.DepthEnable = TRUE;
		depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
		depthStencilDesc.StencilEnable = FALSE;
		depthStencilDesc.StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK;
		depthStencilDesc.StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK;
		const D3D12_DEPTH_STENCILOP_DESC defaultStencilOp = { D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_COMPARISON_FUNC_ALWAYS };
		depthStencilDesc.FrontFace = defaultStencilOp;
		depthStencilDesc.BackFace = defaultStencilOp;

		D3D12_RASTERIZER_DESC rasterizerDesk;
		ZeroMemory(&rasterizerDesk, sizeof(D3D12_RASTERIZER_DESC));
		rasterizerDesk.FillMode = D3D12_FILL_MODE_SOLID;
		rasterizerDesk.CullMode = D3D12_CULL_MODE_BACK;
		rasterizerDesk.DepthClipEnable = FALSE;
		rasterizerDesk.FrontCounterClockwise = TRUE;
		rasterizerDesk.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
		rasterizerDesk.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
		rasterizerDesk.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
		rasterizerDesk.MultisampleEnable = pci.Multisampled;
		rasterizerDesk.AntialiasedLineEnable = FALSE;
		rasterizerDesk.ForcedSampleCount = pci.MultisampleCount;
		rasterizerDesk.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

		std::vector<D3D12_INPUT_ELEMENT_DESC> desc;
		desc.push_back({ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
		desc.push_back({ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, sizeof(float) * 3, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
		desc.push_back({ "TEXCOORDS", 0, DXGI_FORMAT_R32G32_FLOAT, 0, sizeof(float) * 6, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });

		ID3DBlob* vertexShader = reinterpret_cast<ID3DBlob*>(pci.PipelineShader->GetVertexShader());
		ID3DBlob* fragmentShader = reinterpret_cast<ID3DBlob*>(pci.PipelineShader->GetFragmentShader());

		D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesk;
		ZeroMemory(&psoDesk, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
		psoDesk.pRootSignature = reinterpret_cast<ID3D12RootSignature*>(pci.PipelineShader->GetLinkedProgram());
		psoDesk.InputLayout.NumElements = desc.size();
		psoDesk.InputLayout.pInputElementDescs = desc.data();
		psoDesk.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		psoDesk.VS = { reinterpret_cast<UINT8*>(vertexShader->GetBufferPointer()), vertexShader->GetBufferSize() };
		psoDesk.PS = { reinterpret_cast<UINT8*>(fragmentShader->GetBufferPointer()), fragmentShader->GetBufferSize() };
		psoDesk.RasterizerState = rasterizerDesk;
		psoDesk.BlendState.AlphaToCoverageEnable = FALSE;
		psoDesk.BlendState.IndependentBlendEnable = FALSE;
		psoDesk.BlendState.RenderTarget[0] = {
			FALSE,FALSE,
			D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
			D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
			D3D12_LOGIC_OP_NOOP,
			D3D12_COLOR_WRITE_ENABLE_ALL,
		};
		psoDesk.DepthStencilState = depthStencilDesc;
		psoDesk.SampleMask = UINT_MAX;
		psoDesk.NumRenderTargets = 1;
		psoDesk.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		psoDesk.DSVFormat = DXGI_FORMAT_D32_FLOAT;
		psoDesk.SampleDesc.Count = 1;
		psoDesk.SampleDesc.Quality = 0;

		auto res = device->CreateGraphicsPipelineState(&psoDesk, IID_PPV_ARGS(&m_PSO));
		DX12Renderer::CheckIfFailed(res, "D3D12: Failed to create graphics pipeline state!");
	}

	DX12Pipeline::~DX12Pipeline()
	{
		m_PSO->Release();
	}

	void DX12Pipeline::Bind()
	{
		auto clist = DX12Renderer::GetCurrentCommand()->GetCommandList();
		clist->SetPipelineState(m_PSO);
	}

	void DX12Pipeline::Unbind()
	{
		auto clist = DX12Renderer::GetCurrentCommand()->GetCommandList();
		clist->SetPipelineState(nullptr);
	}
}
#include "DX11RasterizerLibrary.h"
#include "DX11Renderer.h"

namespace symphony
{
	std::unordered_map<std::string, ID3D11RasterizerState*> RasterizerLibrary::m_Rasterizers;

	static D3D11_RASTERIZER_DESC SymphonyToD3D11Desc(const RasterizerSpecification& spec)
	{
		D3D11_RASTERIZER_DESC desc{};
		
		D3D11_CULL_MODE mode;
		if (spec.cullMode == CullMode::None)
			mode = D3D11_CULL_NONE;
		if (spec.cullMode == CullMode::Front)
			mode = D3D11_CULL_FRONT;
		else
			mode = D3D11_CULL_BACK;

		D3D11_FILL_MODE fillMode;
		if (spec.fillMode == FillMode::Wireframe)
			fillMode = D3D11_FILL_WIREFRAME;
		else
			fillMode = D3D11_FILL_SOLID;

		desc.CullMode = mode;
		desc.FillMode = fillMode;
		desc.AntialiasedLineEnable = spec.FXAA;
		desc.MultisampleEnable = spec.MSAA;
		desc.FrontCounterClockwise = spec.FrontCounterClockwise;
		desc.DepthClipEnable = true;

		return desc;
	}

	void RasterizerLibrary::InitDefaultRasterizers()
	{
		auto device = DX11Renderer::GetRendererData().Device;

		// Mesh Rasterizer
		{
			RasterizerSpecification mrs{};
			mrs.cullMode = CullMode::Back;
			D3D11_RASTERIZER_DESC d3d11meshDesc = SymphonyToD3D11Desc(mrs);
			ID3D11RasterizerState* temp = nullptr;
			device->CreateRasterizerState(&d3d11meshDesc, &temp);
			m_Rasterizers["Mesh State"] = temp;
		}

		// Skybox Rasterizer
		{
			RasterizerSpecification srs{};
			srs.cullMode = CullMode::Front;
			D3D11_RASTERIZER_DESC d3d11meshDesc = SymphonyToD3D11Desc(srs);
			ID3D11RasterizerState* temp = nullptr;
			device->CreateRasterizerState(&d3d11meshDesc, &temp);
			m_Rasterizers["Skybox State"] = temp;
		}

		// Wireframe Mesh Rasterizer
		{
			RasterizerSpecification wmrs{};
			wmrs.fillMode = FillMode::Wireframe;
			wmrs.cullMode = CullMode::Back;
			D3D11_RASTERIZER_DESC d3d11meshDesc = SymphonyToD3D11Desc(wmrs);
			ID3D11RasterizerState* temp = nullptr;
			device->CreateRasterizerState(&d3d11meshDesc, &temp);
			m_Rasterizers["Mesh Wireframe State"] = temp;
		}
	}

	void RasterizerLibrary::Shutdown()
	{
		for (auto rasterize : m_Rasterizers)
		{
			rasterize.second->Release();
		}
	}

	void RasterizerLibrary::AddRasterizerState(const RasterizerSpecification& spec, const std::string& name)
	{
		auto device = DX11Renderer::GetRendererData().Device;

		D3D11_RASTERIZER_DESC d3d11meshDesc = SymphonyToD3D11Desc(spec);
		ID3D11RasterizerState* temp = nullptr;
		device->CreateRasterizerState(&d3d11meshDesc, &temp);
		m_Rasterizers[name] = temp;
	}

	void RasterizerLibrary::SetRasterizerState(const std::string& name)
	{
		auto ctx = DX11Renderer::GetRendererData().Context;
		ctx->RSSetState(m_Rasterizers[name]);
	}
}
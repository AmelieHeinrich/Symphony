#include "DX11Texture2D.h"
#include "render/ImageData.h"
#include "DX11Renderer.h"

namespace symphony
{
	DX11Texture2D::DX11Texture2D(const char* filepath)
	{
		ImageData imageData = ImageData::LoadImageData(filepath, 2);
		auto device = DX11Renderer::GetRendererData().Device;

		D3D11_SUBRESOURCE_DATA initData;
		initData.pSysMem = imageData.DataBuffer;
		initData.SysMemPitch = 4 * imageData.Width;
		initData.SysMemSlicePitch = imageData.Width * imageData.Height * 4;

		D3D11_TEXTURE2D_DESC desc{};
		desc.Width = imageData.Width;
		desc.Height = imageData.Height;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.CPUAccessFlags = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.SampleDesc.Count = 1;

		HRESULT res = device->CreateTexture2D(&desc, &initData, &m_Texture);
		DX11Renderer::CheckIfFailed(res, "D3D11: Failed to create Texture2D!");

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		ZeroMemory(&srvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
		srvDesc.Format = desc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = desc.MipLevels;

		res = device->CreateShaderResourceView(m_Texture, &srvDesc, &m_ShaderResourceView);
		DX11Renderer::CheckIfFailed(res, "D3D11: Failed to create Texture2D shader resource view!");

		D3D11_SAMPLER_DESC sdesc{};
		sdesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		sdesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		sdesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		sdesc.MinLOD = 0;
		sdesc.MaxLOD = D3D11_FLOAT32_MAX;
		sdesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		sdesc.ComparisonFunc = D3D11_COMPARISON_NEVER;

		res = device->CreateSamplerState(&sdesc, &m_SamplerState);
		DX11Renderer::CheckIfFailed(res, "D3D11: Failed to create Texture2D sampler state!");

		ImageData::FreeImageData(imageData);
	}

	DX11Texture2D::~DX11Texture2D()
	{
		m_SamplerState->Release();
		m_ShaderResourceView->Release();
		m_Texture->Release();
	}

	void DX11Texture2D::Bind(int slot)
	{
		auto context = DX11Renderer::GetRendererData().Context;
		context->PSSetShaderResources(slot, 1, &m_ShaderResourceView);
		context->PSSetSamplers(slot, 1, &m_SamplerState);
	}

	void DX11Texture2D::Unbind(int slot)
	{
		auto context = DX11Renderer::GetRendererData().Context;
		ID3D11ShaderResourceView* rv = nullptr;
		context->PSSetShaderResources(slot, 1, &rv);
	}
}
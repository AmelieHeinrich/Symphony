#include "DX11Texture2D.h"
#include "render/ImageData.h"
#include "DX11Renderer.h"

namespace symphony
{
	DX11Texture2D::DX11Texture2D(const char* filepath, DXGI_FORMAT format)
	{
		ImageData imageData;

		if (format == DXGI_FORMAT_R8G8B8A8_UNORM)
			imageData = ImageData::LoadImageData(filepath, 2);
		if (format == DXGI_FORMAT_R16G16B16A16_FLOAT || format == DXGI_FORMAT_R32G32B32A32_FLOAT)
			imageData = ImageData::LoadFloatImageData(filepath, 2);

		auto device = DX11Renderer::GetRendererData().Device;

		const void* dataBuffer;
		if (format == DXGI_FORMAT_R8G8B8A8_UNORM)
			dataBuffer = imageData.DataBuffer;
		if (format == DXGI_FORMAT_R16G16B16A16_FLOAT || format == DXGI_FORMAT_R32G32B32A32_FLOAT)
			dataBuffer = imageData.FloatDataBuffer;

		int channelSize;
		if (imageData.DataBuffer)
			channelSize = 4;
		if (imageData.FloatDataBuffer)
			channelSize = 16;

		D3D11_SUBRESOURCE_DATA initData;
		initData.pSysMem = dataBuffer;
		initData.SysMemPitch = channelSize * imageData.Width;
		initData.SysMemSlicePitch = imageData.Width * imageData.Height * channelSize;

		D3D11_TEXTURE2D_DESC desc{};
		desc.Width = imageData.Width;
		desc.Height = imageData.Height;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = format;
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

		ImageData::FreeImageData(imageData);
	}

	DX11Texture2D::~DX11Texture2D()
	{
		m_ShaderResourceView->Release();
		m_Texture->Release();
	}

	void DX11Texture2D::Bind(int slot)
	{
		auto context = DX11Renderer::GetRendererData().Context;
		context->PSSetShaderResources(slot, 1, &m_ShaderResourceView);
	}

	void DX11Texture2D::Unbind(int slot)
	{
		auto context = DX11Renderer::GetRendererData().Context;
		ID3D11ShaderResourceView* rv = nullptr;
		context->PSSetShaderResources(slot, 1, &rv);
	}
}
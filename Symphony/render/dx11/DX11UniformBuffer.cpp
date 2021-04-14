#include "DX11UniformBuffer.h"
#include "DX11Renderer.h"

namespace symphony
{
	DX11UniformBuffer::DX11UniformBuffer(void* data, uint32_t size)
	{
		D3D11_BUFFER_DESC buff_desc = {};
		buff_desc.Usage = D3D11_USAGE_DEFAULT;
		buff_desc.ByteWidth = size;
		buff_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		buff_desc.CPUAccessFlags = 0;
		buff_desc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA init_data = {};
		init_data.pSysMem = data;

		auto res = DX11Renderer::GetRendererData().Device->CreateBuffer(&buff_desc, &init_data, &m_BufferHandle);
		DX11Renderer::CheckIfFailed(res, "D3D11: Failed to create UBO!");
	}

	DX11UniformBuffer::~DX11UniformBuffer()
	{
		m_BufferHandle->Release();
	}

	void DX11UniformBuffer::BindForShader(int bufferIndex)
	{
		DX11Renderer::GetRendererData().Context->VSSetConstantBuffers(bufferIndex, 1, &m_BufferHandle);
		DX11Renderer::GetRendererData().Context->PSSetConstantBuffers(bufferIndex, 1, &m_BufferHandle);
	}

	void DX11UniformBuffer::Unbind(int bufferIndex)
	{
		DX11Renderer::GetRendererData().Context->VSSetConstantBuffers(0, 1, nullptr);
		DX11Renderer::GetRendererData().Context->PSSetConstantBuffers(0, 1, nullptr);
	}

	void DX11UniformBuffer::Update(void* data)
	{
		DX11Renderer::GetRendererData().Context->UpdateSubresource(m_BufferHandle, NULL, NULL, data, NULL, NULL);
	}
}
#include "DX11IndexBuffer.h"
#include "DX11Renderer.h"

namespace symphony
{
	DX11IndexBuffer::DX11IndexBuffer(const std::vector<uint16_t>& indices)
		: IndexBuffer(indices)
	{
		D3D11_BUFFER_DESC ibd;
		ZeroMemory(&ibd, sizeof(D3D11_BUFFER_DESC));
		ibd.Usage = D3D11_USAGE_DEFAULT;
		ibd.ByteWidth = indices.size() * sizeof(uint16_t);
		ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		ibd.CPUAccessFlags = 0;
		ibd.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA ibInitData;
		ibInitData.pSysMem = indices.data();

		DX11Renderer::GetRendererData().Device->CreateBuffer(&ibd, &ibInitData, &m_Handle);
	}

	DX11IndexBuffer::~DX11IndexBuffer()
	{
		m_Handle->Release();
	}

	void DX11IndexBuffer::Bind() const
	{
		UINT offset = 0;
		DX11Renderer::GetRendererData().Context->IASetIndexBuffer(m_Handle, DXGI_FORMAT_R16_UINT, offset);
	}

	void DX11IndexBuffer::Unbind() const
	{

	}
}
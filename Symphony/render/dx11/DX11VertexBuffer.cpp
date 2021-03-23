#include "DX11VertexBuffer.h"
#include <string>
#include "DX11Shader.h"
#include "DX11Renderer.h"

namespace symphony
{
	enum class LayoutType
	{
		Float,
		Float2,
		Float3,
		Float4,
		Int,
		Int2,
		Int3,
		Int4
	};

	struct BufferElement
	{
		std::string Name;
		uint32_t SemanticIndex;
		LayoutType Format;
		uint32_t InputSlot;

		uint32_t GetFormatSize() const
		{
			switch (Format)
			{
			case LayoutType::Float:	 return 4;
			case LayoutType::Float2: return 4 * 2;
			case LayoutType::Float3: return 4 * 3;
			case LayoutType::Float4: return 4 * 4;
			case LayoutType::Int:	 return 4;
			case LayoutType::Int2:   return 4 * 2;
			case LayoutType::Int3:   return 4 * 3;
			case LayoutType::Int4:   return 4 * 4;
			}
		}
	};

	static DXGI_FORMAT SymphonyToDXGI(LayoutType type)
	{
		switch (type)
		{
		case LayoutType::Float: return DXGI_FORMAT_R32_FLOAT;
		case LayoutType::Float2: return DXGI_FORMAT_R32G32_FLOAT;
		case LayoutType::Float3: return DXGI_FORMAT_R32G32B32_FLOAT;
		case LayoutType::Float4: return DXGI_FORMAT_R32G32B32A32_FLOAT;
		case LayoutType::Int: return DXGI_FORMAT_R32_SINT;
		case LayoutType::Int2: return DXGI_FORMAT_R32G32_SINT;
		case LayoutType::Int3: return DXGI_FORMAT_R32G32B32_SINT;
		case LayoutType::Int4: return DXGI_FORMAT_R32G32B32A32_SINT;
		}
	};

	static uint32_t GetByteOffset(const std::vector<BufferElement>& vec, int index)
	{
		if (index == 0) return 0;

		uint32_t offset = vec[1].GetFormatSize();
		for (int i = 1; i < index; i++)
		{
			offset += vec[i].GetFormatSize();
		}

		return offset;
	}

	DX11VertexBuffer::DX11VertexBuffer(const std::vector<Vertex>& vertices)
		: VertexBuffer(vertices)
	{
		std::vector<BufferElement> Layout;
		Layout.push_back({ "POSITION", 0, LayoutType::Float3, 0 });
		Layout.push_back({ "COLOR", 0, LayoutType::Float3, 0 });
		Layout.push_back({ "TEXCOORD", 0, LayoutType::Float2, 0 });

		D3D11_INPUT_ELEMENT_DESC* desc = new D3D11_INPUT_ELEMENT_DESC[Layout.size()];

		for (auto i = 0; i < Layout.size(); i++)
		{
			const BufferElement& elem = Layout[i];
			desc[i] = { elem.Name.c_str(), elem.SemanticIndex, SymphonyToDXGI(elem.Format), elem.InputSlot, GetByteOffset(Layout, i), D3D11_INPUT_PER_VERTEX_DATA, 0 };
		}

		const DX11Shader* shader = DX11Shader::CurrentlyBound();
		DX11Renderer::GetRendererData().Device->CreateInputLayout(desc, static_cast<uint32_t>(Layout.size()), shader->GetData().VertexBlob->GetBufferPointer(), shader->GetData().VertexBlob->GetBufferSize(), &m_InputLayout);
		delete[] desc;

		// ACTUAL BUFFER CREATION

		m_BufferDesc = {};
		m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
		m_BufferDesc.ByteWidth = sizeof(Vertex) * static_cast<uint32_t>(vertices.size());
		m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		m_BufferDesc.CPUAccessFlags = 0;
		m_BufferDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA init_data = {};
		init_data.pSysMem = vertices.data();

		DX11Renderer::GetRendererData().Device->CreateBuffer(&m_BufferDesc, &init_data, &m_Buffer);
	}

	DX11VertexBuffer::~DX11VertexBuffer()
	{
		m_Buffer->Release();
		m_InputLayout->Release();
	}

	void DX11VertexBuffer::Bind()
	{
		UINT stride = sizeof(Vertex);
		UINT offset = 0;
		DX11Renderer::GetRendererData().Context->IASetVertexBuffers(0, 1, &m_Buffer, &stride, &offset);
		DX11Renderer::GetRendererData().Context->IASetInputLayout(m_InputLayout);
	}

	void DX11VertexBuffer::Unbind()
	{

	}

	void* DX11VertexBuffer::GetPointerInternal()
	{
		DX11Renderer::GetRendererData().Context->Map(m_Buffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &m_MappedSubresource);
		return m_MappedSubresource.pData;
	}
}
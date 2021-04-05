#include "DX11Shader.h"
#include <fstream>
#include "DX11Renderer.h"
#include <core/Log.h>
#include <sstream>

namespace symphony
{
	static std::string ReadFile(const std::string& filepath)
	{
		std::string result;
		std::ifstream in(filepath, std::ios::in | std::ios::binary); // ifstream closes itself due to RAII
		if (in)
		{
			in.seekg(0, std::ios::end);
			size_t size = in.tellg();
			if (size != -1)
			{
				result.resize(size);
				in.seekg(0, std::ios::beg);
				in.read(&result[0], size);
			}
			else
			{
				SY_CORE_ERROR("Failed to read file buffer!");
			}
		}
		else
		{
			SY_CORE_ERROR("Failed to open file!");
		}

		return result;
	}

	const DX11Shader* DX11Shader::s_CurrentlyBound = nullptr;

	DX11Shader::DX11Shader(const std::string& vertexSource, const std::string& fragmentSource)
		: Shader(vertexSource, fragmentSource)
	{
		auto vertCode = ReadFile(vertexSource);
		auto fragCode = ReadFile(fragmentSource);

		Load(vertCode, fragCode);
	}

	DX11Shader::~DX11Shader()
	{
		m_Data.VertexShader->Release();
		m_Data.FragmentShader->Release();
	}

	void DX11Shader::Bind()
	{
		s_CurrentlyBound = this;

		DX11Renderer::GetRendererData().Context->VSSetShader(m_Data.VertexShader, NULL, 0);
		DX11Renderer::GetRendererData().Context->PSSetShader(m_Data.FragmentShader, NULL, 0);
	}

	void DX11Shader::Unbind()
	{
		s_CurrentlyBound = nullptr;

		DX11Renderer::GetRendererData().Context->VSSetShader(nullptr, NULL, 0);
		DX11Renderer::GetRendererData().Context->PSSetShader(nullptr, NULL, 0);
	}

	ID3DBlob* DX11Shader::Compile(const std::string& source, const std::string& profile, const std::string& main, ShaderErrorInfo& info)
	{
		ID3DBlob* shaderBlob;
		ID3DBlob* errorBlob;
		HRESULT status = D3DCompile(source.c_str(), source.size(), NULL, NULL, NULL, main.c_str(), profile.c_str(), D3DCOMPILE_DEBUG, 0, &shaderBlob, &errorBlob);
		if (status != S_OK)
			info.Message = "Unable to compile shader from source\n";
		if (errorBlob)
		{
			info.Profile += profile + "\n";
			std::stringstream ss;
			if (errorBlob->GetBufferSize())
				ss << "Shader Compile Errors" << std::endl << (const char*)errorBlob->GetBufferPointer() << std::endl;
			errorBlob->Release();
			SY_CORE_ERROR(ss.str());
		}
		if (status == S_OK)
			return shaderBlob;
		return nullptr;
	}

	void DX11Shader::Load(const std::string& vertexSource, const std::string& fragmentSource)
	{
		ShaderErrorInfo info;
		m_Data.VertexBlob = Compile(vertexSource, "vs_5_0", "VSMain", info);
		m_Data.FragmentBlob = Compile(fragmentSource, "ps_5_0", "PSMain", info);
		DX11Renderer::GetRendererData().Device->CreateVertexShader(m_Data.VertexBlob->GetBufferPointer(), m_Data.VertexBlob->GetBufferSize(), NULL, &m_Data.VertexShader);
		DX11Renderer::GetRendererData().Device->CreatePixelShader(m_Data.FragmentBlob->GetBufferPointer(), m_Data.FragmentBlob->GetBufferSize(), NULL, &m_Data.FragmentShader);
	}
}
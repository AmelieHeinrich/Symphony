#include "DX12Shader.h"
#include "DX12Device.h"
#include "DX12Renderer.h"
#include <fstream>

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
				__debugbreak();
			}
		}
		else
		{
			__debugbreak();
		}

		return result;
	}

	DX12Shader::DX12Shader(const std::string& vertexFile, const std::string& fragmentFile)
		: Shader(vertexFile, fragmentFile)
	{
		{
			auto device = DX12Renderer::GetRendererData().RendererDevice->GetDevice();
			D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc;
			rootSignatureDesc.NumParameters = 0;
			rootSignatureDesc.pParameters = nullptr;
			rootSignatureDesc.NumStaticSamplers = 0;
			rootSignatureDesc.pStaticSamplers = nullptr;
			rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

			ID3DBlob* signature;
			ID3DBlob* error;
			DX12Device::ThrowIfFailed(D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error));
			DX12Device::ThrowIfFailed(device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&rootSignature)));
		}

		{
			std::string vertSource = ReadFile(vertexFile);
			std::string fragSource = ReadFile(fragmentFile);

			VertexBlob = Compile(vertSource, "vs_5_0", "VSMain");
			FragmentBlob = Compile(fragSource, "ps_5_0", "PSMain");
		}
	}

	DX12Shader::~DX12Shader()
	{
		VertexBlob->Release();
		FragmentBlob->Release();
		rootSignature->Release();
	}

	void DX12Shader::Bind()
	{
		auto clist = DX12Renderer::GetRendererData().RendererCommand->GetCommandList();
		clist->SetGraphicsRootSignature(rootSignature);
	}

	void DX12Shader::Unbind()
	{
		auto clist = DX12Renderer::GetRendererData().RendererCommand->GetCommandList();
		clist->SetGraphicsRootSignature(nullptr);
	}

	ID3DBlob* DX12Shader::Compile(const std::string& source, const std::string& profile, const std::string& main)
	{
		ID3DBlob* shaderBlob;
		ID3DBlob* errorBlob;
		DX12Device::ThrowIfFailed(D3DCompile(source.c_str(), source.size(), NULL, NULL, NULL, main.c_str(), profile.c_str(), 0, 0, &shaderBlob, &errorBlob));
		return shaderBlob;
	}
}
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

			D3D12_DESCRIPTOR_RANGE  descriptorTableRanges[1]; // only one range right now
			descriptorTableRanges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV; // this is a range of constant buffer views (descriptors)
			descriptorTableRanges[0].NumDescriptors = 1; // we only have one constant buffer, so the range is only 1
			descriptorTableRanges[0].BaseShaderRegister = 0; // start index of the shader registers in the range
			descriptorTableRanges[0].RegisterSpace = 0; // space 0. can usually be zero
			descriptorTableRanges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND; // this appends the range to the end of the root signature descriptor tables

			D3D12_ROOT_DESCRIPTOR_TABLE descriptorTable;
			descriptorTable.NumDescriptorRanges = _countof(descriptorTableRanges); 
			descriptorTable.pDescriptorRanges = &descriptorTableRanges[0];

			// create a root parameter and fill it out
			D3D12_ROOT_PARAMETER  rootParameters[1]; // only one parameter right now
			rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; // this is a descriptor table
			rootParameters[0].DescriptorTable = descriptorTable; // this is our descriptor table for this root parameter
			rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

			auto flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | // we can deny shader stages here for better performance
				D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
				D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
				D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
				D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;

			D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc;
			rootSignatureDesc.NumParameters = _countof(rootParameters);
			rootSignatureDesc.pParameters = rootParameters;
			rootSignatureDesc.NumStaticSamplers = 0;
			rootSignatureDesc.pStaticSamplers = nullptr;
			rootSignatureDesc.Flags = flags;

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
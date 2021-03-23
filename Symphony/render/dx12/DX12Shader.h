#pragma once

#include <d3d12.h>
#include <d3dcompiler.h>
#include "render/Shader.h"

namespace symphony
{
	class DX12Shader : public Shader
	{
	public:
		DX12Shader(const std::string& vertexFile, const std::string& fragmentFile);
		~DX12Shader();

		void Bind();
		void Unbind();

		virtual void* GetVertexShader() override { return nullptr; }
		virtual void* GetFragmentShader() override { return nullptr; }
		virtual void* GetLinkedProgram() override { return (void*)rootSignature; }
	private:
		ID3D12RootSignature* rootSignature;

		ID3DBlob* VertexBlob;
		ID3DBlob* FragmentBlob;

		ID3DBlob* Compile(const std::string& source, const std::string& profile, const std::string& main);
	};
}
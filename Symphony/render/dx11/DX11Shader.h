#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>
#include "core/DLLExport.h"
#include "render/Shader.h"

namespace symphony
{
	struct ShaderErrorInfo
	{
		std::string Profile;
		std::string Message;
	};

	SymphonyClass DX11Shader : public Shader
	{
	public:
		DX11Shader(const std::string& vertexFile, const std::string& fragmentFile);
		~DX11Shader();

		void Bind();
		void Unbind();

		static const DX11Shader* CurrentlyBound() { return s_CurrentlyBound; }
	private:
		static ID3DBlob* Compile(const std::string& source, const std::string& profile, const std::string& main, ShaderErrorInfo& info);
		void Load(const std::string& vertexFile, const std::string& fragmentFile);

		static const DX11Shader* s_CurrentlyBound;

		struct Data
		{
			ID3D11VertexShader* VertexShader;
			ID3D11PixelShader* FragmentShader;
			ID3DBlob* VertexBlob;
			ID3DBlob* FragmentBlob;
		};

		std::string m_FilePath;
		mutable Data m_Data;

	public:
		inline Data& GetData() const { return m_Data; }

		virtual void* GetVertexShader() override {
			return (void*)m_Data.VertexShader;
		}

		virtual void* GetFragmentShader() override {
			return (void*)m_Data.FragmentShader;
		}

		virtual void* GetLinkedProgram() override {
			return nullptr;
		}
	};
}
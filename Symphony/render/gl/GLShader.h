#pragma once

#include "render/Shader.h"
#include "core/DLLExport.h"

namespace symphony
{
	class GLShader : public Shader
	{
	public:
		GLShader(std::string vertexFile, std::string fragmentFile);
		~GLShader();

		void Bind();
		void Unbind();

		virtual void* GetVertexShader() override {
			return nullptr;
		}

		virtual void* GetFragmentShader() override {
			return nullptr;
		}

		virtual void* GetLinkedProgram() override {
			return (void*)m_Program;
		}
	private:
		uint32_t m_Program;
	};
}
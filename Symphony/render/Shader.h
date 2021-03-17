#pragma once

#include "core/DLLExport.h"
#include <string>

namespace symphony
{
	SymphonyClass Shader
	{
	public:
		Shader(std::string vertexFile, std::string fragmentFile) {}
		virtual ~Shader() = default;

		virtual void* GetVertexShader() = 0;
		virtual void* GetFragmentShader() = 0;
		virtual void* GetLinkedProgram() = 0;
	};
}
#include "GLShader.h"
#include <glad/gl.h>
#include <fstream>
#include <iostream>

namespace symphony
{
	static std::string readFile(std::string filepath) {
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
		}

		return result;
	}

	GLShader::GLShader(std::string vertexFile, std::string fragmentFile)
		: Shader(vertexFile, fragmentFile)
	{
		auto vertCodeFile = readFile(vertexFile);
		auto fragCodeFile = readFile(fragmentFile);

		const char* vertCode = vertCodeFile.c_str();
		const char* fragCode = fragCodeFile.c_str();

		unsigned int vertex, fragment;
		int success;
		char infoLog[512];

		// vertex Shader
		vertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex, 1, &vertCode, NULL);
		glCompileShader(vertex);
		// print compile errors if any
		glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(vertex, 512, NULL, infoLog);
			std::cout << infoLog << std::endl;
		};

		// vertex Shader
		fragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment, 1, &fragCode, NULL);
		glCompileShader(fragment);
		// print compile errors if any
		glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(fragment, 512, NULL, infoLog);
			std::cout << infoLog << std::endl;
		};

		// shader Program
		m_Program = glCreateProgram();
		glAttachShader(m_Program, vertex);
		glAttachShader(m_Program, fragment);
		glLinkProgram(m_Program);
		// print linking errors if any
		glGetProgramiv(m_Program, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(m_Program, 512, NULL, infoLog);
		}

		// delete the shaders as they're linked into our program now and no longer necessary
		glDeleteShader(vertex);
		glDeleteShader(fragment);
	}

	GLShader::~GLShader()
	{
		glDeleteProgram(m_Program);
	}

	void GLShader::Bind()
	{
		glUseProgram(m_Program);
	}

	void GLShader::Unbind()
	{
		glUseProgram(0);
	}
}
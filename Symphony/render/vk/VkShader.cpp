#include "VkShader.h"
#include <fstream>
#include "core/exception/VulkanException.h"
#include <iostream>
#include "VkRenderer.h"
#include <shaderc/shaderc.hpp>
#include <core/FileSystem.h>

namespace symphony
{
	VulkanShader::VulkanShader(std::string vertexFile, std::string fragmentFile)
		: m_DeviceCopy(VulkanRenderer::GetData().m_Device->device()), Shader(vertexFile, fragmentFile)
	{
		std::string vertCode;
		std::string fragCode;

		try {
			vertCode = FileSystem::ReadFile(vertexFile);
		}
		catch (std::runtime_error& e) {
			SY_CORE_ERROR(e.what());
		}

		try {
			fragCode = FileSystem::ReadFile(fragmentFile);
		}
		catch (std::runtime_error& e) {
			SY_CORE_ERROR(e.what());
		}

		shaderc::Compiler compiler;
		shaderc::CompileOptions options;

#ifndef _DEBUG
		options.SetOptimizationLevel(shaderc_optimization_level_size);
#endif

		shaderc::SpvCompilationResult vertResult = compiler.CompileGlslToSpv(vertCode.c_str(), shaderc_shader_kind::shaderc_glsl_vertex_shader, "vs", options);
		if (vertResult.GetCompilationStatus() != shaderc_compilation_status_success) {
			SY_CORE_ERROR("Vulkan: Failed to compile vertex shader!");
		}
		std::vector<uint32_t> vertexSPRV;
		vertexSPRV.assign(vertResult.cbegin(), vertResult.cend());

		shaderc::SpvCompilationResult fragResult = compiler.CompileGlslToSpv(fragCode.c_str(), shaderc_shader_kind::shaderc_glsl_fragment_shader, "fs", options);
		if (fragResult.GetCompilationStatus() != shaderc_compilation_status_success) {
			SY_CORE_ERROR("Vulkan: Failed to compile fragment shader!");
		}
		std::vector<uint32_t> fragmentSPRV;
		fragmentSPRV.assign(fragResult.cbegin(), fragResult.cend());

		VkShaderModuleCreateInfo vertCreateInfo{};
		vertCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		vertCreateInfo.codeSize = vertexSPRV.size() * sizeof(uint32_t);
		vertCreateInfo.pCode = reinterpret_cast<const uint32_t*>(vertexSPRV.data());

		if (vkCreateShaderModule(m_DeviceCopy, &vertCreateInfo, nullptr, &m_VertexShader) != VK_SUCCESS) {
			SY_CORE_ERROR("Vulkan: Failed to create vertex shader module!");
		}

		VkShaderModuleCreateInfo fragCreateInfo{};
		fragCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		fragCreateInfo.codeSize = fragmentSPRV.size() * sizeof(uint32_t);
		fragCreateInfo.pCode = reinterpret_cast<const uint32_t*>(fragmentSPRV.data());

		if (vkCreateShaderModule(m_DeviceCopy, &fragCreateInfo, nullptr, &m_FragmentShader) != VK_SUCCESS) {
			SY_CORE_ERROR("Vulkan: Failed to create fragment shader module!");
		}
	}

	VulkanShader::~VulkanShader()
	{
		vkDestroyShaderModule(m_DeviceCopy, m_VertexShader, nullptr);
		vkDestroyShaderModule(m_DeviceCopy, m_FragmentShader, nullptr);
	}
}
#include "VkShader.h"
#include <fstream>
#include "core/exception/VulkanException.h"
#include <iostream>
#include "VkRenderer.h"

static std::vector<char> readFile(const std::string& filename)
{
	std::ifstream file(filename, std::ios::ate | std::ios::binary);

	if (!file.is_open()) {
		throw std::runtime_error("failed to open file!");
	}

	size_t fileSize = (size_t)file.tellg();
	std::vector<char> buffer(fileSize);

	file.seekg(0);
	file.read(buffer.data(), fileSize);

	file.close();

	return buffer;
}

namespace symphony
{
	VulkanShader::VulkanShader(std::string vertexFile, std::string fragmentFile)
		: m_DeviceCopy(VulkanRenderer::GetData().m_Device->device()), Shader(vertexFile, fragmentFile)
	{
		std::vector<char> vertCode;
		std::vector<char> fragCode;

		try {
			vertCode = readFile(vertexFile);
		}
		catch (std::runtime_error& e) {
			std::cout << e.what() << std::endl;
		}

		try {
			fragCode = readFile(fragmentFile);
		}
		catch (std::runtime_error& e) {
			std::cout << e.what() << std::endl;
		}

		VkShaderModuleCreateInfo vertCreateInfo{};
		vertCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		vertCreateInfo.codeSize = vertCode.size();
		vertCreateInfo.pCode = reinterpret_cast<const uint32_t*>(vertCode.data());

		if (vkCreateShaderModule(m_DeviceCopy, &vertCreateInfo, nullptr, &m_VertexShader) != VK_SUCCESS) {
			throw VulkanException("failed to create shader module!");
		}

		VkShaderModuleCreateInfo fragCreateInfo{};
		fragCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		fragCreateInfo.codeSize = fragCode.size();
		fragCreateInfo.pCode = reinterpret_cast<const uint32_t*>(fragCode.data());

		if (vkCreateShaderModule(m_DeviceCopy, &fragCreateInfo, nullptr, &m_FragmentShader) != VK_SUCCESS) {
			throw VulkanException("failed to create shader module!");
		}
	}

	VulkanShader::~VulkanShader()
	{
		vkDestroyShaderModule(m_DeviceCopy, m_VertexShader, nullptr);
		vkDestroyShaderModule(m_DeviceCopy, m_FragmentShader, nullptr);
	}
}
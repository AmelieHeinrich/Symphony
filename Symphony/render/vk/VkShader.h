#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <string>

namespace symphony
{
	class Shader
	{
	public:
		Shader(std::string vertexFile, std::string fragmentFile, const VkDevice& device);
		~Shader();

		const VkShaderModule& GetVertexShader() const {
			return m_VertexShader;
		}

		const VkShaderModule& GetFramgentShader() const {
			return m_FragmentShader;
		}
	private:
		VkShaderModule m_VertexShader;
		VkShaderModule m_FragmentShader;
		const VkDevice& m_DeviceCopy;
	};
}
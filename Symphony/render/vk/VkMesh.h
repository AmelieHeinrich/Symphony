#pragma once

#include <vulkan/vulkan.h>
#include "VkGraphicsPipeline.h"
#include "VkTexture2D.h"
#include <glm/glm.hpp>
#include <memory>
#include "VkVertexBuffer.h"
#include "VkIndexBuffer.h"

namespace symphony
{
	struct Material
	{
		VkDescriptorSet MaterialDSET;
		VkPipeline MaterialPipeline;
		VkPipelineLayout MaterialPipelineLayout;
	};

	struct Mesh
	{
		glm::mat4 TransformMatrix;

		std::shared_ptr<VulkanVertexBuffer> MeshVBO;
		std::shared_ptr<VulkanIndexBuffer> MeshEBO;
	};
}
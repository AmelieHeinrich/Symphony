#pragma once

#include <vulkan/vulkan.h>
#include "VkGraphicsPipeline.h"
#include "VkTexture2D.h"
#include <glm/glm.hpp>
#include <memory>
#include "VkVertexBuffer.h"
#include "VkIndexBuffer.h"
#include "VkDescriptorSet.h"
#include <render/Renderer.h>
#include <render/Mesh.h>

namespace symphony
{
	class VulkanMesh : public Mesh
	{
	public:
		VulkanMesh(ModelData data);
		~VulkanMesh();

		void Recreate(uint32_t width, uint32_t height);
		void Draw(VkCommandBuffer buffer, uint32_t imageIndex);
	private:
		std::shared_ptr<DescriptorSet> MeshDSET;
		std::shared_ptr<VulkanVertexBuffer> MeshVBO;
		std::shared_ptr<VulkanIndexBuffer> MeshEBO;
		std::shared_ptr<VulkanTexture2D> MeshT2D;
	};
}
#include "VkMesh.h"
#include "VkRenderer.h"

namespace symphony
{
	VulkanMesh::VulkanMesh(ModelData data)
		: Mesh(data)
	{
		MeshVBO = std::make_shared<VulkanVertexBuffer>(data.RendererResources.first);
		MeshEBO = std::make_shared<VulkanIndexBuffer>(data.RendererResources.second);
		MeshT2D = std::make_shared<VulkanTexture2D>(data.TextureFilepath);
		MeshDSET = std::make_shared<DescriptorSet>(MeshT2D);
	}

	VulkanMesh::~VulkanMesh()
	{
		MeshT2D.reset();
		MeshEBO.reset();
		MeshVBO.reset();
		MeshDSET.reset();
	}

	void VulkanMesh::Recreate(uint32_t width, uint32_t height)
	{
		MeshDSET.reset();
		MeshDSET = std::make_shared<DescriptorSet>(MeshT2D);
	}

	void VulkanMesh::Draw(VkCommandBuffer buffer, uint32_t imageIndex)
	{
		auto s_Data = VulkanRenderer::GetData();
		std::vector<VkDeviceSize> offsets = { 0 };
		VkBuffer vbo = MeshVBO->GetVulkanBuffer();
		vkCmdBindVertexBuffers(buffer, 0, 1, &vbo, offsets.data());
		vkCmdBindIndexBuffer(buffer, MeshEBO->GetVulkanBuffer(), 0, VK_INDEX_TYPE_UINT32);
		vkCmdBindDescriptorSets(buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, s_Data.graphicsPipeline->GetPipelineLayout(), 0, 1, &MeshDSET->GetDescriptorSet()[imageIndex], 0, nullptr);
		vkCmdDrawIndexed(buffer, MeshEBO->GetIndicesSize(), 1, 0, 0, 0);
	}
}
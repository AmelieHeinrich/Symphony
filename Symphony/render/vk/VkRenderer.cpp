#include "VkRenderer.h"
#include "window/Window.h"
#include <chrono>
#include "render/Renderer.h"
#include "VkGui.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <core/Application.h>

namespace symphony
{
	RendererData VulkanRenderer::s_Data;
	std::unordered_map<std::string, std::shared_ptr<VulkanMesh>> VulkanRenderer::m_Meshes;
	Window* VulkanRenderer::targetWindow;

	void VulkanRenderer::Init(Window* window)
	{
		targetWindow = window;
		int width, height;
		SDL_GetWindowSize(window->GetWindowHandle(), &width, &height);

		s_Data.FBWidth = width;
		s_Data.FBHeight = height;

		s_Data.m_Instance = std::make_shared<Instance>(window->GetWindowHandle());
		s_Data.m_Surface = std::make_shared<Surface>(s_Data.m_Instance->instance(), window->GetWindowHandle());
		s_Data.m_PhysicalDevice = std::make_shared<PhysicalDevice>(s_Data.m_Instance->instance(), s_Data.m_Surface->surface());
		s_Data.m_Device = std::make_shared<Device>(s_Data.m_PhysicalDevice->gpu(), s_Data.m_Surface->surface());
		s_Data.m_SwapChain = std::make_shared<SwapChain>(s_Data.m_PhysicalDevice->gpu(), s_Data.m_Surface->surface(), s_Data.m_Device->device());
		s_Data.m_RenderPass = std::make_shared<RenderPass>(s_Data.m_Device->device(), s_Data.m_SwapChain->swap_chain_image_format());
		s_Data.m_ImGuiPass = std::make_shared<RenderPass>(s_Data.m_Device->device(), s_Data.m_SwapChain->swap_chain_image_format(), false);
		s_Data.descriptorSetLayout = std::make_shared<DescriptorSetLayout>();

		std::shared_ptr<VulkanShader> shader = std::make_shared<VulkanShader>("shaderlib/vksl/Vertex.vert", "shaderlib/vksl/Fragment.frag");

		GraphicsPipelineCreateInfo info;
		info.Width = s_Data.FBWidth;
		info.Height = s_Data.FBHeight;
		info.PipelineDescriptorSetLayout = s_Data.descriptorSetLayout->GetDescriptorSetLayout();
		info.PipelineShader = shader;
		info.PipelineRenderPass = s_Data.m_RenderPass;
		info.SwapChainExtent = s_Data.m_SwapChain->swap_chain_extent();

		s_Data.graphicsPipeline = std::make_shared<GraphicsPipeline>(s_Data.m_Device->device(), info);

		shader.reset();

		QueueFamilyIndices queueFamilyIndices = PhysicalDevice::FindQueueFamilyIndices(s_Data.m_PhysicalDevice->gpu(), s_Data.m_Surface->surface());
		s_Data.m_CommandPool = std::make_shared<CommandPool>(s_Data.m_Device->device(), queueFamilyIndices.graphicsFamily.value());

		VkFormat depthFormat = VulkanTexture2D::FindDepthFormat();
		VulkanTexture2D::CreateImage(s_Data.FBWidth, s_Data.FBHeight, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, s_Data.DepthImage, s_Data.DepthImageMemory);
		s_Data.DepthImageView = VulkanTexture2D::CreateImageView(s_Data.DepthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);
		VulkanTexture2D::TransitionImageLayout(s_Data.DepthImage, depthFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
		s_Data.m_SwapChain->InitFramebuffers(s_Data.m_RenderPass->render_pass());

		auto nrImages = s_Data.m_SwapChain->swap_chain_images().size();
		s_Data.uniformBuffers.resize(nrImages);
		
		for (int i = 0; i < nrImages; i++) {
			s_Data.uniformBuffers[i] = std::make_shared<VulkanUniformBuffer>();
		}

		s_Data.descriptorPool = std::make_shared<DescriptorPool>();

		VulkanGUI::Init();
	}

	void VulkanRenderer::Resize(unsigned int width, unsigned int height)
	{
		vkDeviceWaitIdle(s_Data.m_Device->device());

		s_Data.FBWidth = width;
		s_Data.FBHeight = height;

		vkDestroyImageView(s_Data.m_Device->device(), s_Data.DepthImageView, nullptr);
		vkDestroyImage(s_Data.m_Device->device(), s_Data.DepthImage, nullptr);
		vkFreeMemory(s_Data.m_Device->device(), s_Data.DepthImageMemory, nullptr);
		
		s_Data.m_SwapChain.reset();
		s_Data.commandBuffer.reset();
		s_Data.graphicsPipeline.reset();
		s_Data.m_RenderPass.reset();
		s_Data.m_ImGuiPass.reset();
		s_Data.descriptorPool.reset();

		s_Data.m_SwapChain = std::make_shared<SwapChain>(s_Data.m_PhysicalDevice->gpu(), s_Data.m_Surface->surface(), s_Data.m_Device->device());
		s_Data.m_RenderPass = std::make_shared<RenderPass>(s_Data.m_Device->device(), s_Data.m_SwapChain->swap_chain_image_format());
		s_Data.m_ImGuiPass = std::make_shared<RenderPass>(s_Data.m_Device->device(), s_Data.m_SwapChain->swap_chain_image_format(), false);

		std::shared_ptr<VulkanShader> shader = std::make_shared<VulkanShader>("shaderlib/vksl/Vertex.vert", "shaderlib/vksl/Fragment.frag");

		GraphicsPipelineCreateInfo info;
		info.Width = s_Data.FBWidth;
		info.Height = s_Data.FBHeight;
		info.PipelineDescriptorSetLayout = s_Data.descriptorSetLayout->GetDescriptorSetLayout();
		info.PipelineShader = shader;
		info.PipelineRenderPass = s_Data.m_RenderPass;
		info.SwapChainExtent = s_Data.m_SwapChain->swap_chain_extent();

		s_Data.graphicsPipeline = std::make_shared<GraphicsPipeline>(s_Data.m_Device->device(), info);

		shader.reset();

		VkFormat depthFormat = VulkanTexture2D::FindDepthFormat();
		VulkanTexture2D::CreateImage(s_Data.FBWidth, s_Data.FBHeight, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, s_Data.DepthImage, s_Data.DepthImageMemory);
		s_Data.DepthImageView = VulkanTexture2D::CreateImageView(s_Data.DepthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);
		VulkanTexture2D::TransitionImageLayout(s_Data.DepthImage, depthFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
		s_Data.m_SwapChain->InitFramebuffers(s_Data.m_RenderPass->render_pass());

		s_Data.descriptorPool = std::make_shared<DescriptorPool>();
		s_Data.commandBuffer = std::make_shared<CommandBuffer>(s_Data.m_Device, s_Data.m_CommandPool, false);

		for (auto mesh : m_Meshes)
			mesh.second->Recreate(width, height);
	}

	void VulkanRenderer::Shutdown()
	{
		vkDeviceWaitIdle(s_Data.m_Device->device());

		for (auto mesh : m_Meshes)
			mesh.second.reset();
		m_Meshes.clear();

		for (auto ubo : s_Data.uniformBuffers)
			ubo.reset();
		s_Data.uniformBuffers.clear();

		vkDestroyImageView(s_Data.m_Device->device(), s_Data.DepthImageView, nullptr);
		vkDestroyImage(s_Data.m_Device->device(), s_Data.DepthImage, nullptr);
		vkFreeMemory(s_Data.m_Device->device(), s_Data.DepthImageMemory, nullptr);

		VulkanGUI::Shutdown();

		s_Data.commandBuffer.reset();
		s_Data.m_CommandPool.reset();

		vkDestroyFence(s_Data.m_Device->device(), s_Data.renderFence, nullptr);
		vkDestroySemaphore(s_Data.m_Device->device(), s_Data.renderFinishedSemaphores, nullptr);
		vkDestroySemaphore(s_Data.m_Device->device(), s_Data.imageAvailableSemaphores, nullptr);

		s_Data.graphicsPipeline.reset();
		s_Data.descriptorPool.reset();
		s_Data.descriptorSetLayout.reset();
		s_Data.m_ImGuiPass.reset();
		s_Data.m_RenderPass.reset();
		s_Data.m_SwapChain.reset();
		s_Data.m_Device.reset();
		s_Data.m_PhysicalDevice.reset();
		s_Data.m_Surface.reset();
		s_Data.m_Instance.reset();
	}

	void VulkanRenderer::ClearColor(float r, float g, float b, float a)
	{
		s_Data.ClearColorR = r;
		s_Data.ClearColorG = g;
		s_Data.ClearColorB = b;
		s_Data.ClearColorA = a;
	}

	void VulkanRenderer::Draw()
	{
		vkWaitForFences(s_Data.m_Device->device(), 1, &s_Data.renderFence, VK_TRUE, 1000000000);
		vkResetFences(s_Data.m_Device->device(), 1, &s_Data.renderFence);
		vkResetCommandBuffer(s_Data.commandBuffer->GetCommandBuffer(), 0);

		uint32_t imageIndex;
		vkAcquireNextImageKHR(s_Data.m_Device->device(), s_Data.m_SwapChain->swap_chain(), 1000000000, s_Data.imageAvailableSemaphores, VK_NULL_HANDLE, &imageIndex);

		{
			s_Data.commandBuffer->Begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

			// MAIN PASS
			{
				std::array<VkClearValue, 2> clearColor;
				clearColor[0].color = { s_Data.ClearColorR, s_Data.ClearColorG, s_Data.ClearColorB, s_Data.ClearColorA };
				clearColor[1].depthStencil.depth = 1.0f;
				clearColor[1].depthStencil.stencil = 0;

				VkRenderPassBeginInfo renderPassInfo{};
				renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
				renderPassInfo.renderPass = s_Data.m_RenderPass->render_pass();
				renderPassInfo.framebuffer = s_Data.m_SwapChain->swap_chain_framebuffers()[imageIndex];
				renderPassInfo.renderArea.offset = { 0, 0 };
				renderPassInfo.renderArea.extent = s_Data.m_SwapChain->swap_chain_extent();
				renderPassInfo.clearValueCount = static_cast<uint32_t>(clearColor.size());
				renderPassInfo.pClearValues = clearColor.data();

				vkCmdBeginRenderPass(s_Data.commandBuffer->GetCommandBuffer(), &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
				vkCmdBindPipeline(s_Data.commandBuffer->GetCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, s_Data.graphicsPipeline->GetPipeline());

				int numTris = 0;
				int drawCalls = 0;
				for (auto mesh : m_Meshes) {
					auto model = mesh.second;
					RendererUniforms ubo{};
					ubo.SceneProjection = glm::perspective(glm::radians(45.0f), s_Data.FBWidth / (float)s_Data.FBHeight, 0.01f, 1000.0f);
					ubo.SceneProjection[1][1] *= -1;
					ubo.SceneView = glm::mat4(1.0f);
					ubo.SceneModel = model->GetModelMatrix();

					vkCmdPushConstants(s_Data.commandBuffer->GetCommandBuffer(), s_Data.graphicsPipeline->GetPipelineLayout(), VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(RendererUniforms), &ubo);
					model->Draw(s_Data.commandBuffer->GetCommandBuffer(), imageIndex);

					numTris += model->GetNumberOfVertices() / 3;
					drawCalls++;
				}
				Renderer::Stats.NumTriangles = numTris;
				Renderer::Stats.DrawCalls = drawCalls;
				numTris = 0;
				drawCalls = 0;

				vkCmdEndRenderPass(s_Data.commandBuffer->GetCommandBuffer());
			}

			// IMGUI PASS
			{
				std::array<VkClearValue, 2> clearColor = {};

				VkRenderPassBeginInfo renderPassInfo{};
				renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
				renderPassInfo.renderPass = s_Data.m_ImGuiPass->render_pass();
				renderPassInfo.framebuffer = s_Data.m_SwapChain->swap_chain_framebuffers()[imageIndex];
				renderPassInfo.renderArea.offset = { 0, 0 };
				renderPassInfo.renderArea.extent = s_Data.m_SwapChain->swap_chain_extent();
				renderPassInfo.clearValueCount = 0;
				renderPassInfo.pClearValues = clearColor.data();

				vkCmdBeginRenderPass(s_Data.commandBuffer->GetCommandBuffer(), &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

				VulkanGUI::BeginGUI();
				ImGui::ShowDemoWindow();
				VulkanGUI::EndGUI();

				vkCmdEndRenderPass(s_Data.commandBuffer->GetCommandBuffer());
			}

			s_Data.commandBuffer->End();
		}

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		VkSemaphore waitSemaphores[] = { s_Data.imageAvailableSemaphores };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;

		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &s_Data.commandBuffer->GetCommandBuffer();

		VkSemaphore signalSemaphores[] = { s_Data.renderFinishedSemaphores };
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		vkResetFences(s_Data.m_Device->device(), 1, &s_Data.renderFence);

		if (vkQueueSubmit(s_Data.m_Device->graphics_queue()->queue(), 1, &submitInfo, s_Data.renderFence) != VK_SUCCESS) {
			throw std::runtime_error("failed to submit draw command buffer!");
		}

		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;

		VkSwapchainKHR swapChains[] = { s_Data.m_SwapChain->swap_chain() };
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;

		presentInfo.pImageIndices = &imageIndex;

		vkQueuePresentKHR(s_Data.m_Device->present_queue()->queue(), &presentInfo);

		s_Data.currentFrame = (s_Data.currentFrame + 1) % 2;
	}

	uint32_t VulkanRenderer::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
	{
		VkPhysicalDeviceMemoryProperties memProperties;
		vkGetPhysicalDeviceMemoryProperties(s_Data.m_PhysicalDevice->gpu(), &memProperties);

		for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
			if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
				return i;
			}
		}

		throw std::runtime_error("failed to find suitable memory type!");
	}

	void VulkanRenderer::Prepare()
	{
		s_Data.commandBuffer = std::make_unique<CommandBuffer>(s_Data.m_Device, s_Data.m_CommandPool, false);

		VkSemaphoreCreateInfo semaphoreInfo{};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fenceInfo{};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		if (vkCreateSemaphore(s_Data.m_Device->device(), &semaphoreInfo, nullptr, &s_Data.imageAvailableSemaphores) != VK_SUCCESS ||
			vkCreateSemaphore(s_Data.m_Device->device(), &semaphoreInfo, nullptr, &s_Data.renderFinishedSemaphores) != VK_SUCCESS ||
			vkCreateFence(s_Data.m_Device->device(), &fenceInfo, nullptr, &s_Data.renderFence) != VK_SUCCESS) {
			SY_CORE_ERROR("Vulkan: Failed to create synchronized objects!");
		}
	}

	void VulkanRenderer::AddVertexBuffer(const std::vector<Vertex>& vertices)
	{

	}

	void VulkanRenderer::AddIndexBuffer(const std::vector<uint32_t>& vertices)
	{

	}

	void VulkanRenderer::AddTexture2D(const char* filepath)
	{

	}

	void VulkanRenderer::AddMesh(Mesh mesh, const std::string& name)
	{
		m_Meshes[name] = std::make_shared<VulkanMesh>(mesh.GetModelData());
	}

	void VulkanRenderer::SetMeshTransform(const std::string& meshName, const glm::mat4& transform)
	{
		m_Meshes[meshName]->ModelMatrix = transform;
	}

	void VulkanRenderer::PrintRendererInfo()
	{
		auto info = s_Data.m_PhysicalDevice->gpu();
		VkPhysicalDeviceProperties props;
		vkGetPhysicalDeviceProperties(info, &props);

		SY_CORE_INFO("Vulkan API Version: " + std::to_string(props.apiVersion));
		SY_CORE_INFO("Vulkan Driver Verison: " + std::to_string(props.driverVersion));
		SY_CORE_INFO("Vulkan Vendor ID: " + std::to_string(props.vendorID));
		SY_CORE_INFO("Vulkan Device ID: " + std::to_string(props.deviceID));
		SY_CORE_INFO("Vulkan Device Name: " + std::string(props.deviceName));
	}
}
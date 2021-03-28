#include "VkRenderer.h"
#include "window/Window.h"
#include <chrono>
#include "render/Renderer.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "VkImGui.h"

namespace symphony
{
	RendererData VulkanRenderer::s_Data;
	std::vector<std::shared_ptr<VulkanVertexBuffer>> VulkanRenderer::m_VertexBuffers;
	std::vector<std::shared_ptr<VulkanIndexBuffer>> VulkanRenderer::m_IndexBuffers;
	std::vector<std::shared_ptr<VulkanTexture2D>> VulkanRenderer::m_Textures;
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
		s_Data.descriptorSetLayout = std::make_shared<DescriptorSetLayout>();

		std::shared_ptr<VulkanShader> shader = std::make_shared<VulkanShader>("shaderlib/vksl/Vertex.vert", "shaderlib/vksl/Fragment.frag");

		GraphicsPipelineCreateInfo info;
		info.Width = s_Data.FBWidth;
		info.Height = s_Data.FBHeight;
		info.PipelineDescriptorSetLayout = s_Data.descriptorSetLayout;
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
	}

	void VulkanRenderer::Shutdown()
	{
		vkDeviceWaitIdle(s_Data.m_Device->device());

		for (auto i : m_Textures) {
			i.reset();
		}
		m_Textures.clear();

		for (auto i : s_Data.uniformBuffers) {
			i.reset();
		}
		s_Data.uniformBuffers.clear();

		s_Data.descriptorSet.reset();
		s_Data.descriptorPool.reset();

		for (auto i : m_IndexBuffers) {
			i.reset();
		}
		m_IndexBuffers.clear();

		for (auto i : m_VertexBuffers) {
			i.reset();
		}
		m_VertexBuffers.clear();

		for (size_t i = 0; i < 2; i++) {
			vkDestroySemaphore(s_Data.m_Device->device(), s_Data.renderFinishedSemaphores[i], nullptr);
			vkDestroySemaphore(s_Data.m_Device->device(), s_Data.imageAvailableSemaphores[i], nullptr);
			vkDestroyFence(s_Data.m_Device->device(), s_Data.inFlightFences[i], nullptr);
		}

		vkDestroyImageView(s_Data.m_Device->device(), s_Data.DepthImageView, nullptr);
		vkDestroyImage(s_Data.m_Device->device(), s_Data.DepthImage, nullptr);
		vkFreeMemory(s_Data.m_Device->device(), s_Data.DepthImageMemory, nullptr);

		s_Data.m_CommandPool.reset();

		for (auto i : s_Data.commandBuffers) {
			i.reset();
		}

		s_Data.commandBuffers.clear();

		s_Data.graphicsPipeline.reset();
		s_Data.descriptorSetLayout.reset();
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
		vkWaitForFences(s_Data.m_Device->device(), 1, &s_Data.inFlightFences[s_Data.currentFrame], VK_TRUE, UINT64_MAX);

		uint32_t imageIndex;
		vkAcquireNextImageKHR(s_Data.m_Device->device(), s_Data.m_SwapChain->swap_chain(), UINT64_MAX, s_Data.imageAvailableSemaphores[s_Data.currentFrame], VK_NULL_HANDLE, &imageIndex);
			
		
		RendererUniforms ubo{};
		ubo.SceneProjection = glm::perspective(glm::radians(45.0f), s_Data.FBWidth / (float)s_Data.FBHeight, 0.01f, 1000.0f);
		ubo.SceneView = glm::mat4(1.0f);
		ubo.SceneModel = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.5f, -50.0f)) * glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f)) * glm::rotate(glm::mat4(1.0f), (float)SDL_GetTicks() / 1000.0f, glm::vec3(0.0f, 1.0f, 0.0f));

		s_Data.uniformBuffers[imageIndex]->Update(ubo);

		if (s_Data.imagesInFlight[imageIndex] != VK_NULL_HANDLE) {
			vkWaitForFences(s_Data.m_Device->device(), 1, &s_Data.imagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
		}
		s_Data.imagesInFlight[imageIndex] = s_Data.inFlightFences[s_Data.currentFrame];

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		VkSemaphore waitSemaphores[] = { s_Data.imageAvailableSemaphores[s_Data.currentFrame] };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;

		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &s_Data.commandBuffers[imageIndex]->GetCommandBuffer();

		VkSemaphore signalSemaphores[] = { s_Data.renderFinishedSemaphores[s_Data.currentFrame] };
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		vkResetFences(s_Data.m_Device->device(), 1, &s_Data.inFlightFences[s_Data.currentFrame]);

		if (vkQueueSubmit(s_Data.m_Device->graphics_queue()->queue(), 1, &submitInfo, s_Data.inFlightFences[s_Data.currentFrame]) != VK_SUCCESS) {
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
		s_Data.descriptorSet = std::make_shared<DescriptorSet>();
		s_Data.commandBuffers.resize(s_Data.m_SwapChain->swap_chain_framebuffers().size());

		for (int i = 0; i < s_Data.commandBuffers.size(); i++) {
			s_Data.commandBuffers[i] = std::make_unique<CommandBuffer>(s_Data.m_Device, s_Data.m_CommandPool, false);
		}

		for (size_t i = 0; i < s_Data.commandBuffers.size(); i++) {
			s_Data.commandBuffers[i]->Begin(0);

			std::array<VkClearValue, 2> clearColor;
			clearColor[0].color = { s_Data.ClearColorR, s_Data.ClearColorG, s_Data.ClearColorB, s_Data.ClearColorA };
			clearColor[1].depthStencil.depth = 1.0f;
			clearColor[1].depthStencil.stencil = 0;

			VkRenderPassBeginInfo renderPassInfo{};
			renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			renderPassInfo.renderPass = s_Data.m_RenderPass->render_pass();
			renderPassInfo.framebuffer = s_Data.m_SwapChain->swap_chain_framebuffers()[i];
			renderPassInfo.renderArea.offset = { 0, 0 };
			renderPassInfo.renderArea.extent = s_Data.m_SwapChain->swap_chain_extent();
			renderPassInfo.clearValueCount = static_cast<uint32_t>(clearColor.size());
			renderPassInfo.pClearValues = clearColor.data();

			vkCmdBeginRenderPass(s_Data.commandBuffers[i]->GetCommandBuffer(), &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

			vkCmdBindPipeline(s_Data.commandBuffers[i]->GetCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, s_Data.graphicsPipeline->GetPipeline());

			std::vector<VkBuffer> vertexBuffers;
			std::vector<VkDeviceSize> offsets = { 0 };
			uint32_t finalVertexCount = 0;
	
			std::vector<VkBuffer> indexBuffers;
			uint32_t finalIndexCount = 0;

			for (int i = 0; i < m_VertexBuffers.size(); i++) {
				vertexBuffers.push_back((VkBuffer)m_VertexBuffers[i]->GetVertexBufferHandle());
				finalVertexCount += m_VertexBuffers[i]->GetVerticesSize();
			}

			if (!m_IndexBuffers.empty()) {
				for (int i = 0; i < m_IndexBuffers.size(); i++) {
					indexBuffers.push_back((VkBuffer)m_IndexBuffers[i]->GetIndexBufferHandle());
					finalIndexCount += m_IndexBuffers[i]->GetIndicesSize();
				}
			}

			vkCmdBindVertexBuffers(s_Data.commandBuffers[i]->GetCommandBuffer(), 0, 1, vertexBuffers.data(), offsets.data());
			vkCmdBindDescriptorSets(s_Data.commandBuffers[i]->GetCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, s_Data.graphicsPipeline->GetPipelineLayout(), 0, 1, &s_Data.descriptorSet->GetDescriptorSet()[i], 0, nullptr);

			if (m_IndexBuffers.empty()) {
				vkCmdDraw(s_Data.commandBuffers[i]->GetCommandBuffer(), finalVertexCount, 1, 0, 0);
			}
			else {
				for (auto buffer : indexBuffers) {
					vkCmdBindIndexBuffer(s_Data.commandBuffers[i]->GetCommandBuffer(), buffer, 0, VK_INDEX_TYPE_UINT32);
				}

				vkCmdDrawIndexed(s_Data.commandBuffers[i]->GetCommandBuffer(), finalIndexCount, 1, 0, 0, 0);
			}

			vkCmdEndRenderPass(s_Data.commandBuffers[i]->GetCommandBuffer());

			s_Data.commandBuffers[i]->End();
		}

		s_Data.imageAvailableSemaphores.resize(s_Data.m_SwapChain->swap_chain_images().size());
		s_Data.renderFinishedSemaphores.resize(s_Data.m_SwapChain->swap_chain_images().size());
		s_Data.inFlightFences.resize(s_Data.m_SwapChain->swap_chain_images().size());
		s_Data.imagesInFlight.resize(s_Data.m_SwapChain->swap_chain_images().size(), VK_NULL_HANDLE);

		VkSemaphoreCreateInfo semaphoreInfo{};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fenceInfo{};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		for (size_t i = 0; i < 2; i++) {
			if (vkCreateSemaphore(s_Data.m_Device->device(), &semaphoreInfo, nullptr, &s_Data.imageAvailableSemaphores[i]) != VK_SUCCESS ||
				vkCreateSemaphore(s_Data.m_Device->device(), &semaphoreInfo, nullptr, &s_Data.renderFinishedSemaphores[i]) != VK_SUCCESS ||
				vkCreateFence(s_Data.m_Device->device(), &fenceInfo, nullptr, &s_Data.inFlightFences[i]) != VK_SUCCESS) {
				throw VulkanException("failed to create synchronization objects for a frame!");
			}
		}
	}

	void VulkanRenderer::AddVertexBuffer(const std::vector<Vertex>& vertices)
	{
		m_VertexBuffers.push_back(std::make_shared<VulkanVertexBuffer>(vertices));
	}

	void VulkanRenderer::AddIndexBuffer(const std::vector<uint32_t>& indices)
	{
		m_IndexBuffers.push_back(std::make_shared<VulkanIndexBuffer>(indices));
	}

	void VulkanRenderer::AddTexture2D(const char* filepath)
	{
		m_Textures.push_back(std::make_shared<VulkanTexture2D>(filepath));
	}
}
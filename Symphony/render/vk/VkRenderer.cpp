#include "VkRenderer.h"
#include "window/Window.h"

namespace symphony
{
	RendererData VulkanRenderer::s_Data;
	std::vector<std::shared_ptr<VulkanVertexBuffer>> VulkanRenderer::m_VertexBuffers;
	std::vector<std::shared_ptr<VulkanIndexBuffer>> VulkanRenderer::m_IndexBuffers;

	void VulkanRenderer::Init(Window* window)
	{
		int width, height;
		SDL_GetWindowSize(window->GetWindowHandle(), &width, &height);

		s_Data.m_Instance = std::make_shared<Instance>(window->GetWindowHandle());
		s_Data.m_Surface = std::make_shared<Surface>(s_Data.m_Instance->instance(), window->GetWindowHandle());
		s_Data.m_PhysicalDevice = std::make_shared<PhysicalDevice>(s_Data.m_Instance->instance(), s_Data.m_Surface->surface());
		s_Data.m_Device = std::make_shared<Device>(s_Data.m_PhysicalDevice->gpu(), s_Data.m_Surface->surface());
		s_Data.m_SwapChain = std::make_shared<SwapChain>(s_Data.m_PhysicalDevice->gpu(), s_Data.m_Surface->surface(), s_Data.m_Device->device());
		s_Data.m_RenderPass = std::make_shared<RenderPass>(s_Data.m_Device->device(), s_Data.m_SwapChain->swap_chain_image_format());

		std::shared_ptr<Shader> shader = std::make_shared<Shader>("shaderlib/vksl/Vertex.spv", "shaderlib/vksl/Fragment.spv", s_Data.m_Device->device());

		GraphicsPipelineCreateInfo info;
		info.Width = 1280;
		info.Height = 720;
		info.PipelineShader = shader;
		info.PipelineRenderPass = s_Data.m_RenderPass;
		info.SwapChainExtent = s_Data.m_SwapChain->swap_chain_extent();

		s_Data.graphicsPipeline = std::make_shared<GraphicsPipeline>(s_Data.m_Device->device(), info);

		shader.reset();

		s_Data.m_SwapChain->InitFramebuffers(s_Data.m_RenderPass->render_pass());

		QueueFamilyIndices queueFamilyIndices = PhysicalDevice::FindQueueFamilyIndices(s_Data.m_PhysicalDevice->gpu(), s_Data.m_Surface->surface());

		s_Data.m_CommandPool = std::make_shared<CommandPool>(s_Data.m_Device->device(), queueFamilyIndices.graphicsFamily.value());
	}

	void VulkanRenderer::Shutdown()
	{
		vkDeviceWaitIdle(s_Data.m_Device->device());

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

		s_Data.m_CommandPool.reset();

		for (auto i : s_Data.commandBuffers) {
			i.reset();
		}

		s_Data.commandBuffers.clear();

		s_Data.graphicsPipeline.reset();
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
		s_Data.commandBuffers.resize(s_Data.m_SwapChain->swap_chain_framebuffers().size());

		for (int i = 0; i < s_Data.commandBuffers.size(); i++) {
			s_Data.commandBuffers[i] = std::make_unique<CommandBuffer>(s_Data.m_Device, s_Data.m_CommandPool, false);
		}

		for (size_t i = 0; i < s_Data.commandBuffers.size(); i++) {
			s_Data.commandBuffers[i]->Begin(0);

			VkRenderPassBeginInfo renderPassInfo{};
			renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			renderPassInfo.renderPass = s_Data.m_RenderPass->render_pass();
			renderPassInfo.framebuffer = s_Data.m_SwapChain->swap_chain_framebuffers()[i];

			renderPassInfo.renderArea.offset = { 0, 0 };
			renderPassInfo.renderArea.extent = s_Data.m_SwapChain->swap_chain_extent();

			VkClearValue clearColor;
			clearColor.color = { s_Data.ClearColorR, s_Data.ClearColorG, s_Data.ClearColorB, s_Data.ClearColorA };

			renderPassInfo.clearValueCount = 1;
			renderPassInfo.pClearValues = &clearColor;

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
			
			if (m_IndexBuffers.empty()) {
				vkCmdDraw(s_Data.commandBuffers[i]->GetCommandBuffer(), finalVertexCount, 1, 0, 0);
			}
			else {
				for (auto buffer : indexBuffers) {
					vkCmdBindIndexBuffer(s_Data.commandBuffers[i]->GetCommandBuffer(), buffer, 0, VK_INDEX_TYPE_UINT16);
				}

				vkCmdDrawIndexed(s_Data.commandBuffers[i]->GetCommandBuffer(), finalIndexCount, 1, 0, 0, 0);
			}

			vkCmdEndRenderPass(s_Data.commandBuffers[i]->GetCommandBuffer());

			s_Data.commandBuffers[i]->End();
		}

		s_Data.imageAvailableSemaphores.resize(2);
		s_Data.renderFinishedSemaphores.resize(2);
		s_Data.inFlightFences.resize(2);
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

	void VulkanRenderer::AddVertexBuffer(std::shared_ptr<VulkanVertexBuffer> vertexBuffer)
	{
		m_VertexBuffers.push_back(vertexBuffer);
	}

	void VulkanRenderer::AddIndexBuffer(std::shared_ptr<VulkanIndexBuffer> indexBuffer)
	{
		m_IndexBuffers.push_back(indexBuffer);
	}
}
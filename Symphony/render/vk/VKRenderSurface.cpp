#include "VKRenderSurface.h"

#include <SDL.h>
#include "VkInstance.h"
#include "VkSurface.h"
#include "VkPhysicalDevice.h"
#include "VkDevice.h"
#include "VkSwapChain.h"
#include "VkRenderPass.h"
#include "VkCommandPool.h"
#include "VkShader.h"
#include "core/exception/VulkanException.h"

const int MAX_FRAMES_IN_FLIGHT = 2;

namespace symphony {

	void VKRenderSurface::Init(void* extraparams) {
		m_TargetWindow = (SDL_Window*)extraparams;
		int width, height;
		SDL_GetWindowSize(m_TargetWindow, &width, &height);

		m_Instance = std::make_shared<Instance>(m_TargetWindow);
		m_Surface = std::make_shared<Surface>(m_Instance->instance(), m_TargetWindow);
		m_PhysicalDevice = std::make_shared<PhysicalDevice>(m_Instance->instance(), m_Surface->surface());
		m_Device = std::make_shared<Device>(m_PhysicalDevice->gpu(), m_Surface->surface());
		m_SwapChain = std::make_shared<SwapChain>(m_PhysicalDevice->gpu(), m_Surface->surface(), m_Device->device());
		m_RenderPass = std::make_shared<RenderPass>(m_Device->device(), m_SwapChain->swap_chain_image_format());

		std::shared_ptr<Shader> shader = std::make_shared<Shader>("shaderlib/vksl/Vertex.spv", "shaderlib/vksl/Fragment.spv", m_Device->device());

		GraphicsPipelineCreateInfo info;
		info.Width = 1280;
		info.Height = 720;
		info.PipelineShader = shader;
		info.PipelineRenderPass = m_RenderPass;
		info.SwapChainExtent = m_SwapChain->swap_chain_extent();

		graphicsPipeline = std::make_shared<GraphicsPipeline>(m_Device->device(), info);

		shader.reset();

		m_SwapChain->InitFramebuffers(m_RenderPass->render_pass());

		QueueFamilyIndices queueFamilyIndices = PhysicalDevice::FindQueueFamilyIndices(m_PhysicalDevice->gpu(), m_Surface->surface());

		m_CommandPool = std::make_shared<CommandPool>(m_Device->device(), queueFamilyIndices.graphicsFamily.value());

		commandBuffers.resize(m_SwapChain->swpa_chain_framebuffers().size());

		for (int i = 0; i < commandBuffers.size(); i++) {
			commandBuffers[i] = std::make_unique<CommandBuffer>(m_Device, m_CommandPool, false);
		}

		for (size_t i = 0; i < commandBuffers.size(); i++) {
			commandBuffers[i]->Begin(0);

			VkRenderPassBeginInfo renderPassInfo{};
			renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			renderPassInfo.renderPass = m_RenderPass->render_pass();
			renderPassInfo.framebuffer = m_SwapChain->swpa_chain_framebuffers()[i];

			renderPassInfo.renderArea.offset = { 0, 0 };
			renderPassInfo.renderArea.extent = m_SwapChain->swap_chain_extent();

			VkClearValue clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
			renderPassInfo.clearValueCount = 1;
			renderPassInfo.pClearValues = &clearColor;

			vkCmdBeginRenderPass(commandBuffers[i]->GetCommandBuffer(), &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
			vkCmdBindPipeline(commandBuffers[i]->GetCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline->GetPipeline());
			vkCmdDraw(commandBuffers[i]->GetCommandBuffer(), 3, 1, 0, 0);
			vkCmdEndRenderPass(commandBuffers[i]->GetCommandBuffer());
			commandBuffers[i]->End();
		}

		imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
		renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
		inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
		imagesInFlight.resize(m_SwapChain->swap_chain_images().size(), VK_NULL_HANDLE);

		VkSemaphoreCreateInfo semaphoreInfo{};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fenceInfo{};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
			if (vkCreateSemaphore(m_Device->device(), &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
				vkCreateSemaphore(m_Device->device(), &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
				vkCreateFence(m_Device->device(), &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS) {
				throw VulkanException("failed to create synchronization objects for a frame!");
			}
		}
	}

	void VKRenderSurface::Shutdown() {
		vkDeviceWaitIdle(m_Device->device());

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
			vkDestroySemaphore(m_Device->device(), renderFinishedSemaphores[i], nullptr);
			vkDestroySemaphore(m_Device->device(), imageAvailableSemaphores[i], nullptr);
			vkDestroyFence(m_Device->device(), inFlightFences[i], nullptr);
		}

		m_CommandPool.reset();
		
		for (auto i : commandBuffers) {
			i.reset();
		}

		commandBuffers.clear();

		graphicsPipeline.reset();
		m_RenderPass.reset();
		m_SwapChain.reset();
		m_Device.reset();
		m_PhysicalDevice.reset();
		m_Surface.reset();
		m_Instance.reset();
	}

	void VKRenderSurface::Clear() {
		vkWaitForFences(m_Device->device(), 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

		uint32_t imageIndex;
		vkAcquireNextImageKHR(m_Device->device(), m_SwapChain->swap_chain(), UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

		if (imagesInFlight[imageIndex] != VK_NULL_HANDLE) {
			vkWaitForFences(m_Device->device(), 1, &imagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
		}
		imagesInFlight[imageIndex] = inFlightFences[currentFrame];

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		VkSemaphore waitSemaphores[] = { imageAvailableSemaphores[currentFrame] };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;

		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffers[imageIndex]->GetCommandBuffer();

		VkSemaphore signalSemaphores[] = { renderFinishedSemaphores[currentFrame] };
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		vkResetFences(m_Device->device(), 1, &inFlightFences[currentFrame]);

		if (vkQueueSubmit(m_Device->graphics_queue()->queue(), 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS) {
			throw std::runtime_error("failed to submit draw command buffer!");
		}

		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;

		VkSwapchainKHR swapChains[] = { m_SwapChain->swap_chain() };
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;

		presentInfo.pImageIndices = &imageIndex;

		vkQueuePresentKHR(m_Device->present_queue()->queue(), &presentInfo);

		currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
	}

	void VKRenderSurface::ClearColor(float r, float g, float b, float a) {

	}

	void VKRenderSurface::ResizeBuffers(int x, int y, int w, int h) {

	}
}
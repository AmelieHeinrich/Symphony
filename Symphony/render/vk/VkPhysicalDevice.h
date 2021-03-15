#pragma once

#include <vulkan/vulkan.h>
#include <optional>
#include <cstdint>
#include <string>
#include <sstream>
#include <set>
#include <vector>

namespace symphony
{
	struct QueueFamilyIndices {
		std::optional<uint32_t> graphicsFamily;
		std::optional<uint32_t> presentFamily;

		bool isComplete() {
			return graphicsFamily.has_value() && presentFamily.has_value();
		}
	};

	struct SwapChainSupportDetails {
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};

	const std::vector<const char*> DeviceExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};


	class PhysicalDevice
	{
	public:
		PhysicalDevice(VkInstance inst, VkSurfaceKHR surface);
		~PhysicalDevice();

		static bool IsDeviceSuitable(VkPhysicalDevice gpu, VkSurfaceKHR surface);
		static bool CheckDeviceExtensionSupport(VkPhysicalDevice gpu);
		static SwapChainSupportDetails QuerySwapChain(VkPhysicalDevice gpu, VkSurfaceKHR surface);
		static QueueFamilyIndices FindQueueFamilyIndices(VkPhysicalDevice gpu, VkSurfaceKHR surface);

		const VkPhysicalDevice& gpu() const {
			return m_GPU;
		}

		QueueFamilyIndices gpu_queue_families() const {
			return gpuIndices;
		}

		operator const VkPhysicalDevice& () const {
			return m_GPU;
		}
	private:
		VkInstance& instance;
		VkPhysicalDevice m_GPU;
		QueueFamilyIndices gpuIndices;
	};
}

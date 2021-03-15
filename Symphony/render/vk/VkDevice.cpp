#include "VkDevice.h"
#include "core/exception/VulkanException.h"

namespace symphony
{
	Device::Device(VkPhysicalDevice gpu, VkSurfaceKHR surface)
		: m_GPU(gpu)
	{
		QueueFamilyIndices indices = PhysicalDevice::FindQueueFamilyIndices(gpu, surface);

		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };

		float queuePriority = 1.0f;
		for (uint32_t queueFamily : uniqueQueueFamilies) {
			VkDeviceQueueCreateInfo queueCreateInfo{};
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex = queueFamily;
			queueCreateInfo.queueCount = 1;
			queueCreateInfo.pQueuePriorities = &queuePriority;
			queueCreateInfos.push_back(queueCreateInfo);
		}

		VkPhysicalDeviceFeatures deviceFeatures{};

		VkDeviceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

		createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
		createInfo.pQueueCreateInfos = queueCreateInfos.data();

		createInfo.pEnabledFeatures = &deviceFeatures;

		createInfo.enabledExtensionCount = static_cast<uint32_t>(DeviceExtensions.size());
		createInfo.ppEnabledExtensionNames = DeviceExtensions.data();

		if (vkCreateDevice(gpu, &createInfo, nullptr, &m_DeviceHandle) != VK_SUCCESS) {
			throw VulkanException("Failed to create logical device");
		}

		GraphicsQueue = std::make_shared<Queue>(m_DeviceHandle, indices.graphicsFamily.value());
		PresentQueue = std::make_shared<Queue>(m_DeviceHandle, indices.presentFamily.value());
	}

	Device::~Device()
	{
		GraphicsQueue.reset();
		PresentQueue.reset();
		vkDestroyDevice(m_DeviceHandle, nullptr);
	}
}
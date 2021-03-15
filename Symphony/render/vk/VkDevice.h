#pragma once

#include <vulkan/vulkan.h>
#include "VkPhysicalDevice.h"
#include "VkQueue.h"
#include <memory>

namespace symphony
{
	class Device
	{
	public:
		Device(VkPhysicalDevice gpu, VkSurfaceKHR surface);
		~Device();

		const VkDevice& device() const {
			return m_DeviceHandle;
		}

		std::shared_ptr<Queue> graphics_queue() const {
			return GraphicsQueue;
		}

		std::shared_ptr<Queue> present_queue() const {
			return PresentQueue;
		}

		operator const VkDevice& () const {
			return m_DeviceHandle;
		}
	private:
		VkDevice m_DeviceHandle;
		VkPhysicalDevice m_GPU;

		std::shared_ptr<Queue> GraphicsQueue;
		std::shared_ptr<Queue> PresentQueue;
	};
}
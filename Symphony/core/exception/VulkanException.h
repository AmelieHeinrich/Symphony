#pragma once

#include <stdexcept>

namespace symphony {

	class VulkanException : public std::exception
	{
	public:
		VulkanException(const char* message) : message(message) {

		}

		const char* what() const throw()
		{
			return message;
		}
	private:
		const char* message;
	};

}
#pragma once

#include <stdexcept>

namespace symphony {

	class NetworkException : public std::exception
	{
	public:
		NetworkException(const char* message) : message(message) {

		}

		const char* what() const throw()
		{
			return message;
		}
	private:
		const char* message;
	};

}
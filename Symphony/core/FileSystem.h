#pragma once

#include <fstream>
#include <string>

namespace symphony
{
	// TODO : Make a better FileSystem

	enum class ReadMode
	{
		Normal,
		Binary
	};

	class FileSystem
	{
	public:
		static std::string ReadFile(const std::string& filepath, ReadMode readmode);
	};
}
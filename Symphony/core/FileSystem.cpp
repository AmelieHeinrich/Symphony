#include "FileSystem.h"
#include <vector>
#include <sstream>
#include <core/Log.h>

namespace symphony
{
	std::string FileSystem::ReadFile(const std::string& filepath, ReadMode readmode)
	{
		std::ifstream file(filepath, std::ios::ate | std::ios::binary);

		if (!file.is_open()) {
			SY_CORE_ERROR("Failed to open file!");
		}

		size_t fileSize = (size_t)file.tellg();
		std::vector<char> buffer(fileSize);

		file.seekg(0);
		file.read(buffer.data(), fileSize);

		file.close();

		std::stringstream ss;
		for (char i : buffer) {
			ss << i;
		}

		return ss.str();
	}
}


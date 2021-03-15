#include "Plugin.h"
#include <iostream>

namespace symphony {
	IDLLPlugin::IDLLPlugin(const char* path) {
		m_WinInstance = LoadLibraryA(path);

		if (!m_WinInstance) {
			std::cout << "Failed to load DLL plugin with path " << path << std::endl;
		}
	}

	IDLLPlugin::~IDLLPlugin() {
		m_WinInstance = nullptr;
	}

	template<typename T>
	T* IDLLPlugin::LoadFunctionPointer(const char* name)
	{
		T* funcPTR = GetProcAddress(m_WinInstance, name);
		if (!funcPTR) {
			std::cout << "Unable to find function name!" << std::endl;
			return nullptr;
		}

		return funcPTR;
	}
}
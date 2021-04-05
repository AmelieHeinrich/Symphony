#include "Plugin.h"
#include <iostream>
#include <core/Log.h>

namespace symphony {
	IDLLPlugin::IDLLPlugin(const char* path) {
		m_WinInstance = LoadLibraryA(path);

		if (!m_WinInstance) {
			SY_CORE_ERROR("Failed to load DLL plugin!");
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
			SY_CORE_ERROR("Failed to load function pointer!");
			return nullptr;
		}

		return funcPTR;
	}
}
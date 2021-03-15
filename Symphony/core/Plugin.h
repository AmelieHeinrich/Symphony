#pragma once

#include "DLLExport.h"
#include <Windows.h>

namespace symphony {
	class IPlugin
	{
	public:
		virtual ~IPlugin() {}
	};

	SymphonyClass IDLLPlugin : public IPlugin
	{
	public:
		IDLLPlugin(const char* dllPath);
		~IDLLPlugin();

		template<typename T>
		T* LoadFunctionPointer(const char* name);
	private:
		HINSTANCE m_WinInstance;
	};
}
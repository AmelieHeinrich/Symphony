#pragma once

#include "DLLExport.h"
#include <Windows.h>

namespace symphony {
	SymphonyClass IPlugin
	{
	public:
		virtual ~IPlugin() {}
	};

	// DLL Loading using the Win 32 api
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
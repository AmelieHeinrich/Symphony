#pragma once

#include <Windows.h>
#include "window/Window.h"
#include <core/Log.h>

// DLL main to trace dll attachment

HMODULE Win32DLLHandle;

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved)
{
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		Win32DLLHandle = hinstDLL;
		symphony::Window::InitialiseSDL2();
		break;
	case DLL_PROCESS_DETACH:
		Win32DLLHandle = nullptr;
		symphony::Window::TerminateSDL2();
		break;
	}

	return TRUE;
}
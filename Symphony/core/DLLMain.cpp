#pragma once

#include <Windows.h>
#include "window/Window.h"
#include <iostream>

// DLL main to trace dll attachment

HMODULE Win32DLLHandle;

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved)
{
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		Win32DLLHandle = hinstDLL;
		std::cout << "Symphony DLL Injected" << std::endl;
		symphony::Window::InitialiseSDL2();
		break;
	case DLL_PROCESS_DETACH:
		Win32DLLHandle = nullptr;
		std::cout << "Symphony DLL Detached" << std::endl;
		symphony::Window::TerminateSDL2();
		break;
	}

	return TRUE;
}
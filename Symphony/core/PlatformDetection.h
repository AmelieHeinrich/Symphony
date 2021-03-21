#pragma once

// Classic platform detection using macros

#ifdef _WIN64
	#define SYMPHONY_WINDOWS
#else
	#error "32 bit windows architecture is not supported!"
#endif

#ifdef __APPLE__
	#include <TargetConditionals.h>
	#if TARGET_IPHONE_SIMULATOR
		#define SYMPHONY_IOS
	#elif TARGET_OS_IPHONE
		#define SYMPHONY_IOS
	#elif TARGET_OS_MAC
		#define SYMPHONY_MAC
	#else
		#error "Unknown Apple platform"
	#endif
#endif

#ifdef __linux__
	#define SYMPHONY_LINUX
#endif

#include <iostream>

// Wrappers around those macros

namespace symphony {
	enum class Platform {
		Windows,
		iOS,
		MacOS,
		Linux
	};

	enum class RenderAPI {
		None = 0,
		OpenGL = 1,
		DirectX = 2,
		Metal = 3,
		Vulkan = 4
	};

	static Platform s_Platform;

	static const Platform GetCurrentPlatform()
	{
#ifdef SYMPHONY_WINDOWS
		s_Platform = Platform::Windows;
#elif SYMPHONY_IOS
		s_Platform = Platform::iOS;
#elif SYMPHONY_MAC
		s_Platform = Platform::MacOS;
#elif SYMPHONY_LINUX
		s_Platform = Platform::Linux;
#endif

		return (const Platform)s_Platform;
	}
}
#pragma once

// Right now i'm using the standard way of importing dll functions, no platform specific stuff

#ifdef SYMPHONY_BUILD
	#define SymphonyAPI __declspec(dllexport)
	#define SymphonyClass class __declspec(dllexport)
	#define SymphonyStruct struct __declspec(dllexport)
#else
	#define SymphonyAPI __declspec(dllimport)
	#define SymphonyClass class __declspec(dllimport)
	#define SymphonyStruct struct __declspec(dllimport)
#endif
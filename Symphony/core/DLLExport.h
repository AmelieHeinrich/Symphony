#pragma once

#ifdef SYMPHONY_BUILD
	#define SymphonyAPI __declspec(dllexport)
	#define SymphonyClass class __declspec(dllexport)
	#define SymphonyStruct struct __declspec(dllexport)
#else
	#define SymphonyAPI __declspec(dllimport)
	#define SymphonyClass class __declspec(dllimport)
	#define SymphonyStruct struct __declspec(dllimport)
#endif
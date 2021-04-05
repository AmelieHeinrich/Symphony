#pragma once
#include <core/PlatformDetection.h>
#include <core/Log.h>
#include <core/Application.h>

#ifdef SYMPHONY_WINDOWS

int main(int argc, char** argv)
{
	symphony::Log::Init();

	auto app = symphony::CreateApplication();
	app->Run();
	delete app;
}

#endif
#include "Application.h"

#include "render/Renderer.h"
#include "core/Log.h"

namespace symphony
{
	Application* Application::s_Instance = nullptr;

	Application::Application(RenderAPI api, const std::string& name)
	{
		Assert::IsTrue(s_Instance, "Application already exists!");
		s_Instance = this;
		m_Window = std::make_unique<Window>(1280, 720, name.c_str(), api);

		Renderer::Init(m_Window.get());

		SY_CORE_INFO("Application created!");
	}

	Application::~Application()
	{
		Renderer::Shutdown();
		SY_CORE_INFO("Application destroyed!");
	}

	void Application::Close()
	{
		m_Running = false;
	}
}
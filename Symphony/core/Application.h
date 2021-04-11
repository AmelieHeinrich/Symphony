#pragma once

#include <window/Window.h>
#include <core/Assert.h>
#include <core/DLLExport.h>
#include <core/ThreadPool.h>
#undef main

int main(int argc, char** argv);

namespace symphony
{
	SymphonyClass Application
	{
	public:
		Application(RenderAPI renderAPI, const std::string& name = "Symphony App");
		virtual ~Application();

		Window& GetWindow() { return *m_Window; }
		ThreadPool& GetThreadPool() { return *m_ThreadPool; }

		void Close();

		static Application& Get() { return *s_Instance; }
	protected:
		virtual void Run() = 0;
	protected:
		std::unique_ptr<Window> m_Window;
		std::unique_ptr<ThreadPool> m_ThreadPool;
		bool m_Running = true;
	private:
		static Application* s_Instance;
		friend int ::main(int argc, char** argv);
	};

	Application* CreateApplication();
}
#include <Symphony.h>
#include <iostream>
#include <memory>

using namespace symphony;

class SymphonySandbox : public Application
{
public:
	SymphonySandbox()
		: Application(RenderAPI::Vulkan, "Symphony Sandbox")
	{
		Renderer::ClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		ModelData gun = MeshBuilder::LoadModelDataAsync("resources/low.obj", "resources/gun_texture.png");

		Renderer::AddModelData(gun);
		Renderer::Prepare();
	}

	void Run() override
	{
		while (m_Window->IsWindowOpen())
		{
			m_Window->Update();
			Renderer::Draw();
		}
	}
};

Application* symphony::CreateApplication()
{
	return new SymphonySandbox();
}
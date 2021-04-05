#include <Symphony.h>
#include <iostream>
#include <memory>
#include <SDL.h>

using namespace symphony;

class SymphonySandbox : public Application
{
public:
	SymphonySandbox()
		: Application(RenderAPI::OpenGL, "Symphony Sandbox")
	{
		Renderer::ClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		Mesh gunMesh(MeshBuilder::LoadModelDataAsync("resources/low.obj", "resources/gun_texture.png"));
		Mesh monkeyMesh(MeshBuilder::LoadModelDataAsync("resources/suzanne.obj", "resources/marble.jpg"));

		Renderer::AddMesh(gunMesh, "Gun");
		Renderer::AddMesh(monkeyMesh, "Suzanne");
		Renderer::Prepare();
	}

	void Run() override
	{
		while (m_Window->IsWindowOpen())
		{
			glm::mat4 newMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-25.0f, 0.5f, -70.0f)) * glm::rotate(glm::mat4(1.0f), (float)SDL_GetTicks() / 1000.0f, glm::vec3(0.0f, 1.0f, 0.0f));
			glm::mat4 monkeyMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(1.5f, 0.3f, -5.0f)) * glm::rotate(glm::mat4(1.0f), (float)SDL_GetTicks() / 1000.0f, glm::vec3(0.0f, 1.0f, 0.0f));
			Renderer::SetMeshTransform("Gun", newMatrix);
			Renderer::SetMeshTransform("Suzanne", monkeyMatrix);

			m_Window->Update();
			Renderer::Draw();
		}
	}
};

Application* symphony::CreateApplication()
{
	return new SymphonySandbox();
}
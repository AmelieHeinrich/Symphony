#include <Symphony.h>
#include <iostream>
#include <memory>
#include <SDL.h>
#include <stdlib.h>

using namespace symphony;

class SymphonySandbox : public Application
{
public:
	// Choose the API you want (D3D11, GL 4.5 or Vulkan)
	SymphonySandbox()
		: Application(RenderAPI::Vulkan, "Symphony Sandbox")
	{
		// Print the renderer info (DX12 not supported yet)
		Renderer::PrintRendererInfo();

		// Clear color and load the meshes (3D models are loaded on another thread)
		Renderer::ClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		Mesh gunMesh(MeshBuilder::LoadModelDataAsync("resources/low.obj", "resources/gun_texture.png"));
		Mesh monkeyMesh(MeshBuilder::LoadModelDataAsync("resources/suzanne.obj", "resources/marble.jpg"));

		// Send the meshes to the renderer and prepare the renderer for drawing
		Renderer::AddMesh(gunMesh, "Gun");
		Renderer::AddMesh(monkeyMesh, "Suzanne");
		Renderer::Prepare();
	}

	void Run() override
	{
		// Game loop
		while (m_Window->IsWindowOpen())
		{
			// Update the meshes's transform
			glm::mat4 newMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-25.0f, 0.5f, -70.0f)) * glm::rotate(glm::mat4(1.0f), (float)SDL_GetTicks() / 1000.0f, glm::vec3(0.0f, 1.0f, 0.0f));
			glm::mat4 monkeyMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(1.5f, 0.3f, -5.0f)) * glm::rotate(glm::mat4(1.0f), (float)SDL_GetTicks() / 1000.0f, glm::vec3(0.0f, 1.0f, 0.0f));
			Renderer::SetMeshTransform("Gun", newMatrix);
			Renderer::SetMeshTransform("Suzanne", monkeyMatrix);

			m_Window->Update();
			
			// Draw
			Renderer::Draw();
		}
	}
};

// Entry point defined in client
Application* symphony::CreateApplication()
{
	return new SymphonySandbox();
}
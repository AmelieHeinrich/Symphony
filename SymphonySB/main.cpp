#include <Symphony.h>
#include <iostream>
#include <memory>
#include <SDL.h>
#include <stdlib.h>
#include <imgui.h>
#include "FirstPersonCamera.h"

using namespace symphony;

class SymphonySandbox : public Application
{
public:
	SymphonySandbox()
		: Application(RenderAPI::DirectX11, "Symphony Sandbox")
	{
		m_Window->SetEventCallback(SY_BIND_EVENT_FN(SymphonySandbox::OnEvent));

		Renderer::PrintRendererInfo();

		Renderer::ClearColor(0.0156862745f, 0.54509803921f, 0.60392156862f, 1.0f);
		Mesh gunMesh(MeshBuilder::LoadModelData("resources/models/viking_room.obj", "resources/textures/viking_room.png"));
		Mesh monkeyMesh(MeshBuilder::LoadModelData("resources/models/suzanne.obj", "resources/textures/darkMarble.jpg"));

		Renderer::AddMesh(gunMesh, "Gun");
		Renderer::AddMesh(monkeyMesh, "Suzanne");
		Renderer::Prepare();
	}

	void Run() override
	{
		while (m_Window->IsWindowOpen())
		{
			//m_Window->LockMouse(true);

			// Update camera
			{
				if (Input::IsKeyPressed(Key::W))
					camera.ProcessKeyboard(FORWARD, 1);
				if (Input::IsKeyPressed(Key::S))
					camera.ProcessKeyboard(BACKWARD, 1);
				if (Input::IsKeyPressed(Key::A))
					camera.ProcessKeyboard(LEFT, 1);
				if (Input::IsKeyPressed(Key::D))
					camera.ProcessKeyboard(RIGHT, 1);
			}

			Renderer::SetCamera(camera.GetViewMatrix());

			glm::mat4 newMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, -0.3f, -3.0f)) * glm::rotate(glm::mat4(1.0f), (float)SDL_GetTicks() / 1000.0f, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			glm::mat4 monkeyMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(1.5f, 0.3f, -3.0f)) * glm::rotate(glm::mat4(1.0f), (float)SDL_GetTicks() / 1000.0f, glm::vec3(0.0f, 1.0f, 0.0f));
			Renderer::SetMeshTransform("Gun", newMatrix);
			Renderer::SetMeshTransform("Suzanne", monkeyMatrix);

			m_Window->Update();
			Renderer::Draw();
		}
	}

	void OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<MouseMovedEvent>(SY_BIND_EVENT_FN(SymphonySandbox::OnMouseMoved));
	}

	bool OnMouseMoved(MouseMovedEvent& e)
	{
		int xpos = e.GetX();
		int ypos = e.GetY();

		if (firstMouse)
		{
			lastX = xpos;
			lastY = ypos;
			firstMouse = false;
		}

		float xoffset = xpos - lastX;
		float yoffset = lastY - ypos; 

		lastX = xpos;
		lastY = ypos;

		camera.ProcessMouseMovement(xoffset, yoffset);

		return true;
	}
private:
	Camera camera = Camera(glm::vec3(0.0f, 0.0f, 3.0f));
	float lastX = m_Window->GetWidth() / 2.0f;
	float lastY = m_Window->GetHeight() / 2.0f;
	bool firstMouse = true;
};

Application* symphony::CreateApplication()
{
	return new SymphonySandbox();
}
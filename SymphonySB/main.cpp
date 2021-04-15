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
		InternalTimer::Init();

		m_Window->SetEventCallback(SY_BIND_EVENT_FN(SymphonySandbox::OnEvent));

		Renderer::PrintRendererInfo();

		Renderer::ClearColor(0.0156862745f, 0.54509803921f, 0.60392156862f, 1.0f);
		Mesh stanford_dragon(MeshBuilder::LoadModelData("resources/models/dragon.obj", "resources/textures/texture.jpg"));

		MaterialUniforms ubo{};
		ubo.Shininess = 32.0f;

		Renderer::AddRenderObject(stanford_dragon, ubo, "Stanford Dragon");
		Renderer::Prepare();

		Renderer::SetSkybox("resources/skybox/skybox.hdr");

		lightAppInfo = {};
		lightAppInfo.CutOff = glm::cos(glm::radians(12.5f));
		lightAppInfo.OuterCutOff = glm::cos(glm::radians(17.5f));
		lightAppInfo.Ambient = glm::vec3(0.1f, 0.1f, 0.1f);
		lightAppInfo.Diffuse = glm::vec3(0.8f, 0.8f, 0.8f);
		lightAppInfo.Specular = glm::vec3(1.0f, 1.0f, 1.0f);
		lightAppInfo.Constant = 1.0f;
		lightAppInfo.Linear = 0.09f;
		lightAppInfo.Quadratic = 0.032f;
	}

	void Run() override
	{
		while (m_Window->IsWindowOpen())
		{
			InternalTimer::Update();

			// Update camera
			{
				if (Input::IsKeyPressed(Key::W))
					camera.ProcessKeyboard(FORWARD, InternalTimer::GetDeltaTime());
				if (Input::IsKeyPressed(Key::S))
					camera.ProcessKeyboard(BACKWARD, InternalTimer::GetDeltaTime());
				if (Input::IsKeyPressed(Key::A))
					camera.ProcessKeyboard(LEFT, InternalTimer::GetDeltaTime());
				if (Input::IsKeyPressed(Key::D))
					camera.ProcessKeyboard(RIGHT, InternalTimer::GetDeltaTime());
			}

			Renderer::SendCameraPosition(camera.Position);
			Renderer::SetCamera(camera.GetViewMatrix());

			glm::mat4 dragon_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(-0.0f, -0.0f, -3.0f)) * glm::rotate(glm::mat4(1.0f), (float)SDL_GetTicks() / 1000.0f, glm::vec3(0.0f, 1.0f, 0.0f));
			Renderer::SetMeshTransform("Stanford Dragon", dragon_matrix);

			lightAppInfo.CameraPosition = camera.Position;
			lightAppInfo.LightPosition = camera.Position;
			lightAppInfo.LightDirection = camera.Front;

			Renderer::SetLightInformation(lightAppInfo);

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
	LightInformation lightAppInfo;
	Camera camera = Camera(glm::vec3(0.0f, 0.0f, 3.0f));
	float lastX = m_Window->GetWidth() / 2.0f;
	float lastY = m_Window->GetHeight() / 2.0f;
	bool firstMouse = true;
};

Application* symphony::CreateApplication()
{
	return new SymphonySandbox();
}
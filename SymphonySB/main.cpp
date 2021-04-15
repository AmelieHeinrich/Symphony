#include <Symphony.h>
#include <iostream>
#include <memory>
#include <SDL.h>
#include <stdlib.h>
#include <imgui.h>
#include "FirstPersonCamera.h"
#include "ThirdPersonCamera.h"

using namespace symphony;

class SymphonySandbox : public Application
{
public:
	SymphonySandbox()
		: Application(RenderAPI::DirectX11, "Symphony Sandbox"), camera(45.0f, 1280.0f / 720.0f, 0.01f, 100.0f)
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

			camera.OnUpdate(InternalTimer::GetDeltaTime());

			Renderer::SetCamera(camera.GetViewMatrix(), camera.GetProjectionMatrix());
			Renderer::SendCameraPosition(camera.GetPosition());
			camera.SetViewportSize(m_Window->GetWidth(), m_Window->GetHeight());

			glm::mat4 dragon_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(-0.0f, -5.0f, -3.0f)) * glm::rotate(glm::mat4(1.0f), (float)SDL_GetTicks() / 1000.0f, glm::vec3(0.0f, 1.0f, 0.0f));
			Renderer::SetMeshTransform("Stanford Dragon", dragon_matrix);

			lightAppInfo.CameraPosition = camera.GetPosition();
			lightAppInfo.LightPosition = camera.GetPosition();
			lightAppInfo.LightDirection = camera.GetForwardDirection();

			Renderer::SetLightInformation(lightAppInfo);

			m_Window->Update();
			Renderer::Draw();
		}
	}

	void OnEvent(Event& e)
	{
		camera.OnEvent(e);
	}
private:
	LightInformation lightAppInfo;
	EditorCamera camera;
};

Application* symphony::CreateApplication()
{
	return new SymphonySandbox();
}
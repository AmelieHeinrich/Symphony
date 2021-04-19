#include <Symphony.h>
#include <iostream>
#include <memory>
#include <SDL.h>
#include <stdlib.h>
#include <imgui.h>
#include "FirstPersonCamera.h"
#include "ThirdPersonCamera.h"
#include <glm/gtc/type_ptr.hpp>

using namespace symphony;

class SymphonySandbox : public Application
{
public:
	SymphonySandbox()
		: Application(RenderAPI::DirectX11, "Symphony Sandbox"), camera(45.0f, 1920.0f / 1080.0f, 0.01f, 10000.0f)
	{
		InternalTimer::Init();

		m_Window->SetEventCallback(SY_BIND_EVENT_FN(SymphonySandbox::OnEvent));

		Renderer::PrintRendererInfo();

		std::unordered_map<std::string, MaterialTextureType> modelTextures =
		{ {"resources/textures/texture.jpg", MaterialTextureType::Albedo} };

		Renderer::ClearColor(0.0156862745f, 0.54509803921f, 0.60392156862f, 1.0f);
		Mesh stanford_dragon(MeshBuilder::LoadModelData("resources/models/dragon.obj", modelTextures));

		MaterialUniforms ubo{};
		ubo.Ambient = glm::vec3(1.0f);
		ubo.Diffuse = glm::vec3(1.0f);
		ubo.Specular = glm::vec3(1.0f);
		ubo.Shininess = 1.0f;

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

			Renderer::SetCamera(camera.GetViewMatrix(), camera.GetProjectionMatrix());
			Renderer::SendCameraPosition(camera.GetPosition());

			glm::mat4 dragon_matrix = glm::translate(glm::mat4(1.0f), m_DragonTranslation);
			Renderer::SetMeshTransform("Stanford Dragon", dragon_matrix);

			lightAppInfo.CameraPosition = camera.GetPosition();
			lightAppInfo.LightPosition = camera.GetPosition();
			lightAppInfo.LightDirection = glm::eulerAngles(camera.GetOrientation());

			Renderer::Draw();

			GUI::BeginGUI();
			ImGui::SetCurrentContext(GUI::GetCurrentContext());
			
			// VIEWPORT
			GUI::ShowViewport();

			if (GUI::IsViewportFocused())
			{
				camera.ProcessInput(InternalTimer::GetDeltaTime());
			}
			camera.OnUpdate(InternalTimer::GetDeltaTime());

			// LIGHT INFO
			ImGui::Begin("Light Information");
			ImGui::SliderFloat3("Light Ambient", glm::value_ptr(lightAppInfo.Ambient), 0, 1);
			ImGui::SliderFloat3("Light Diffuse", glm::value_ptr(lightAppInfo.Diffuse), 0, 1);
			ImGui::SliderFloat3("Light Specular", glm::value_ptr(lightAppInfo.Specular), 0, 1);
			ImGui::SliderFloat("Light Constant", &lightAppInfo.Constant, 0, 1);
			ImGui::SliderFloat("Light Linear", &lightAppInfo.Linear, 0, 1);
			ImGui::SliderFloat("Light Quadratic", &lightAppInfo.Quadratic, 0, 1);
			ImGui::End();

			// DRAGON TRANSFORM
			ImGui::Begin("Dragon Transform");
			ImGui::SliderFloat3("Translation", glm::value_ptr(m_DragonTranslation), -10, 10);
			ImGui::End();

			GUI::EndGUI();

			Renderer::SetLightInformation(lightAppInfo);

			m_Window->Update();
			Renderer::EndDraw();
		}
	}

	void OnEvent(Event& e)
	{
		if (GUI::IsViewportFocused())
		{
			camera.OnEvent(e);
		}
	}
private:
	LightInformation lightAppInfo;
	EditorCamera camera;
	glm::vec3 m_DragonTranslation = glm::vec3{ -0.0f, -5.0f, -3.0f };
};

Application* symphony::CreateApplication()
{
	return new SymphonySandbox();
}
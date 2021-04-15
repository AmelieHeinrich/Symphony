#include "DX11Gui.h"
#include "core/Application.h"
#include "DX11Renderer.h"
#include <d3d11.h>

namespace symphony
{
	void DX11Gui::Init()
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
		io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleViewports;

		ImGui::StyleColorsDark();

		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		SDL_Window* window = Application::Get().GetWindow().GetWindowHandle();

		ID3D11Device* device = DX11Renderer::GetRendererData().Device;
		ID3D11DeviceContext* ctx = DX11Renderer::GetRendererData().Context;

		ImGui_ImplSDL2_InitForD3D(window);
		ImGui_ImplDX11_Init(device, ctx);
	}

	void DX11Gui::Shutdown()
	{
		ImGui_ImplDX11_Shutdown();
		ImGui_ImplSDL2_Shutdown();
		ImGui::DestroyContext();
	}

	void DX11Gui::BeginGUI()
	{
		SDL_Window* window = Application::Get().GetWindow().GetWindowHandle();
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplSDL2_NewFrame(window);
		ImGui::NewFrame();
	}

	void DX11Gui::EndGUI()
	{
		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		ImGuiIO& io = ImGui::GetIO(); (void)io;
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}
	}
}
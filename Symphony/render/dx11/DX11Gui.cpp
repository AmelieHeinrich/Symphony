#include "DX11Gui.h"
#include "core/Application.h"
#include "DX11Renderer.h"
#include <d3d11.h>

namespace symphony
{
	bool DX11Gui::m_ViewportFocused = true;

	void DX11Gui::Init()
	{
		ImGui::CreateContext();

		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

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

		static bool dockspaceOpen = true;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->Pos);
		ImGui::SetNextWindowSize(viewport->Size);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace", &dockspaceOpen, window_flags);
		ImGui::PopStyleVar(3);

		// DockSpace
		ImGuiIO& io = ImGui::GetIO();
		ImGuiStyle& style = ImGui::GetStyle();
		float minWinSizeX = style.WindowMinSize.x;
		style.WindowMinSize.x = 270.0f;
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}
		style.WindowMinSize.x = minWinSizeX;
	}

	void DX11Gui::EndGUI()
	{
		ImGui::End();

		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		ImGuiIO& io = ImGui::GetIO(); (void)io;
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}
	}

	void DX11Gui::ShowViewport()
	{
		static bool show = true;
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
		ImGui::Begin("Viewport", &show, ImGuiWindowFlags_NoCollapse);

		m_ViewportFocused = ImGui::IsWindowFocused();

		ImVec2 viewportSize = ImGui::GetContentRegionAvail();
		if (DX11Renderer::GetRendererData().FBWidth != viewportSize.x || DX11Renderer::GetRendererData().FBHeight != viewportSize.y)
		{
			DX11Renderer::GetRendererData().CurrentFramebuffer->Resize(viewportSize.x, viewportSize.y);
			DX11Renderer::GetRendererData().FBWidth = viewportSize.x;
			DX11Renderer::GetRendererData().FBHeight = viewportSize.y;
		}

		ImGui::Image(reinterpret_cast<void*>(DX11Renderer::GetRendererData().CurrentFramebuffer->GetShaderResourceView()), ImVec2{ (float)DX11Renderer::GetRendererData().FBWidth, (float)DX11Renderer::GetRendererData().FBHeight });
		ImGui::End();
		ImGui::PopStyleVar();
	}
}
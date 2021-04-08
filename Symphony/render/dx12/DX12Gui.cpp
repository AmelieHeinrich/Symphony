#include "DX12Gui.h"
#include "core/Application.h"
#include "DX12Renderer.h"
#include "DX12HeapManager.h"

namespace symphony
{
	void DX12Gui::Init()
	{
		unsigned char* pixels;
		int width, height;
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking

		ImGui::StyleColorsDark();

		DX12RendererData data = DX12Renderer::GetRendererData();

		SDL_Window* window = Application::Get().GetWindow().GetWindowHandle();

		auto s_ImGuiSamplerHeap = DX12HeapManager::ShaderHeap;

		ImGui_ImplSDL2_InitForD3D(window);
		ImGui_ImplDX12_Init(data.RendererDevice->GetDevice(), 2,
			DXGI_FORMAT_R8G8B8A8_UNORM, s_ImGuiSamplerHeap->GetDescriptorHeap(),
			s_ImGuiSamplerHeap->GetHeapHandle(),
			s_ImGuiSamplerHeap->GetGPUHandle());

		ImGui_ImplDX12_CreateDeviceObjects();
	}

	void DX12Gui::Shutdown()
	{
		ImGui_ImplDX12_InvalidateDeviceObjects();
		ImGui_ImplDX12_Shutdown();
		ImGui_ImplSDL2_Shutdown();
		ImGui::DestroyContext();
	}

	void DX12Gui::BeginGUI()
	{
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.DisplaySize = ImVec2(DX12Renderer::GetRendererData().FBWidth, DX12Renderer::GetRendererData().FBHeight);

		SDL_Window* window = Application::Get().GetWindow().GetWindowHandle();
		ImGui_ImplSDL2_NewFrame(window);
		ImGui_ImplDX12_NewFrame();
		ImGui::NewFrame();
	}

	void DX12Gui::EndGUI()
	{
		ImGui::Render();
		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), DX12Renderer::GetRendererData().RendererCommand->GetCommandList());
	}
}
#include "GUI.h"
#include "Renderer.h"
#include "dx11/DX11Gui.h"

namespace symphony
{
	void GUI::BeginGUI()
	{
		switch (Renderer::s_RenderAPI)
		{
		case RenderAPI::DirectX11:
			DX11Gui::BeginGUI();
			break;
		}
	}

	void GUI::EndGUI()
	{
		switch (Renderer::s_RenderAPI)
		{
		case RenderAPI::DirectX11:
			DX11Gui::EndGUI();
		}
	}
}
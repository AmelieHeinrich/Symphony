#pragma once

#include <d3d11.h>
#include <string>
#include <unordered_map>

namespace symphony
{
	enum class FillMode
	{
		Wireframe,
		Solid
	};

	enum class CullMode
	{
		None,
		Front,
		Back
	};

	struct RasterizerSpecification
	{
		FillMode fillMode = FillMode::Solid;
		CullMode cullMode = CullMode::None;
		bool FrontCounterClockwise = true;
		bool MSAA = false;
		bool FXAA = false;
	};

	class RasterizerLibrary
	{
	public:
		static void InitDefaultRasterizers();
		static void Shutdown();

		static void AddRasterizerState(const RasterizerSpecification& spec, const std::string& name);
		static void SetRasterizerState(const std::string& name);
		
	private:
		static std::unordered_map<std::string, ID3D11RasterizerState*> m_Rasterizers;
	};
}
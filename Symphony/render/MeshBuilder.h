#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>

#include "VertexBuffer.h"
#include <vector>
#include <utility>
#include "core/DLLExport.h"
#include <unordered_map>
#include <thread>
#include <mutex>
#include <future>
#include "Renderer.h"

namespace symphony
{
	struct MTLData
	{
		std::string ambient_texname;             // map_Ka
		std::string diffuse_texname;             // map_Kd
		std::string specular_texname;            // map_Ks
		std::string specular_highlight_texname;  // map_Ns
		std::string bump_texname;                // map_bump, map_Bump, bump
		std::string displacement_texname;        // disp
		std::string alpha_texname;               // map_d
		std::string reflection_texname;          // refl
		std::string roughness_texname;  // map_Pr
		std::string metallic_texname;   // map_Pm
		std::string sheen_texname;      // map_Ps
		std::string emissive_texname;   // map_Ke
		std::string normal_texname;     // norm. For normal mapping.

		double roughness;            // [0, 1] default 0
		double metallic;             // [0, 1] default 0
		double sheen;                // [0, 1] default 0
		double clearcoat_thickness;  // [0, 1] default 0
		double clearcoat_roughness;  // [0, 1] default 0

		double ambient[3];
		double diffuse[3];
		double specular[3];
		double transmittance[3];
		double emission[3];
		double shininess;
		double ior;       // index of refraction
		double dissolve;  // 1 == opaque; 0 == fully transparent
	};

	SymphonyClass MeshBuilder
	{
	public:
		static ModelData LoadModelData(const char* meshFile, const std::unordered_map<std::string, MaterialTextureType>& textures, bool loadMaterials = false);
		static std::vector<MTLData> LoadMTLData(const char* meshFile);
	private:
		static std::pair<std::vector<Vertex>, std::vector<uint32_t>> CreateModelFromOBJ(const char* filepath, bool loadMaterials = false);
		static ModelData LoadModelDataInternalOBJ(const char* meshFile, const std::unordered_map<std::string, MaterialTextureType>& textures, bool loadMaterials = false);
	};
}
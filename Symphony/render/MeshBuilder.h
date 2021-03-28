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

namespace symphony
{
	struct ModelData;

	SymphonyClass MeshBuilder
	{
	public:
		// Can only load .objs for now
		static std::pair<std::vector<Vertex>, std::vector<uint32_t>> CreateModelFromOBJ(const char* filepath);
		static std::vector<Vertex> CreateModelFromOBJVertexOnly(const char* filepath);
		static ModelData LoadModelData(const char* meshFile, const char* textureFile);
	};
}
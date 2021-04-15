#pragma once

#include "Material.h"
#include "Mesh.h"
#include <string>

namespace symphony
{
	class RenderObject
	{
		MaterialUniforms Material;
		ModelData MeshData;

		glm::mat4 ModelMatrix;
	};
}
#pragma once

#include <glm/glm.hpp>
#include "render/Renderer.h"
#include "core/DLLExport.h"

namespace symphony
{
	SymphonyClass Mesh
	{
	public:
		Mesh(ModelData data);
		~Mesh() = default;

		glm::mat4 ModelMatrix;

		const glm::mat4& GetModelMatrix() const {
			return ModelMatrix;
		}

		ModelData GetModelData() { return m_Data; }
	private:
		ModelData m_Data;
	};
}
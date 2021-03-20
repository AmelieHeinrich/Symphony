#pragma once

#include <glad/gl.h>
#include <cstdint>
#include "core/DLLExport.h"

namespace symphony
{
	struct RendererUniforms;

	SymphonyClass GLUniformBuffer
	{
	public:
		GLUniformBuffer(uint32_t program);
		~GLUniformBuffer();

		void Update(RendererUniforms uniform);
	private:
		uint32_t m_RenderID;
	};
}
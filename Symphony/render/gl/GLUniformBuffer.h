#pragma once

#include <glad/gl.h>
#include <cstdint>
#include "core/DLLExport.h"

namespace symphony
{
	/*
		NOTE: I am not using standard uniforms (glUniform) but uniform buffers to stick with the renderer architecture I chose
	*/

	struct RendererUniforms;

	SymphonyClass GLUniformBuffer
	{
	public:
		GLUniformBuffer(uint32_t program);
		~GLUniformBuffer();

		void Update(RendererUniforms uniform);

		// Except for this func
		void SetUniformSampler(uint32_t program);
		void SetCurrentTexture(uint32_t program, uint32_t slot);
	private:
		uint32_t m_RenderID;
	};
}
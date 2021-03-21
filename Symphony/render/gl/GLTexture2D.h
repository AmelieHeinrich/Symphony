#pragma once

#include "render/Texture.h"
#include <glad/gl.h>
#include <cstdint>

namespace symphony
{
	// Since i'm actually trash at writing good opengl code for texture creations i'm using most of Hazel's code for this...
	// COPYRIGHT THE CHERNO : https://github.com/TheCherno/Hazel

	class GLTexture2D : public Texture
	{
	public:
		GLTexture2D(const char* filepath);
		~GLTexture2D();

		void Bind(uint32_t slot = 0);
		void Unbind();

		virtual void* GetTextureBuffer() override {
			return (void*)m_RenderID;
		}
	private:
		uint32_t m_RenderID;
	};
}
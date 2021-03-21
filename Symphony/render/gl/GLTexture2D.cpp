#include "GLTexture2D.h"
#include "render/ImageData.h"
#include <iostream>
#include "stb_image.h"

namespace symphony
{
	GLTexture2D::GLTexture2D(const char* filepath)
	{
		ImageData imageData = ImageData::LoadImageData(filepath, 2);
		
		GLenum internalFormat = 0, dataFormat = 0;
		if (imageData.Channels == 4)
		{
			internalFormat = GL_RGBA8;
			dataFormat = GL_RGBA;
		}
		else if (imageData.Channels == 3)
		{
			internalFormat = GL_RGBA8;
			dataFormat = GL_RGBA;
		}

		glCreateTextures(GL_TEXTURE_2D, 1, &m_RenderID);
		glTextureStorage2D(m_RenderID, 1, internalFormat, imageData.Width, imageData.Height);

		glTextureParameteri(m_RenderID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_RenderID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTextureParameteri(m_RenderID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(m_RenderID, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTextureSubImage2D(m_RenderID, 0, 0, 0, imageData.Width, imageData.Height, dataFormat, GL_UNSIGNED_BYTE, imageData.DataBuffer);

		ImageData::FreeImageData(imageData);
	}

	GLTexture2D::~GLTexture2D()
	{
		glDeleteTextures(1, &m_RenderID);
	}

	void GLTexture2D::Bind(uint32_t slot)
	{
		glBindTextureUnit(slot, m_RenderID);
	}

	void GLTexture2D::Unbind()
	{
		glBindTextureUnit(0, 0);
	}
}
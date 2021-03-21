#pragma once

namespace symphony
{
	// Mother class
	class Texture
	{
	public:
		virtual ~Texture() = default;

		virtual void* GetTextureBuffer() = 0;
	};
}
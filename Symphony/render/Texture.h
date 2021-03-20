#pragma once

namespace symphony
{
	class Texture
	{
	public:
		virtual ~Texture() = default;

		virtual void* GetTextureBuffer() = 0;
	};
}
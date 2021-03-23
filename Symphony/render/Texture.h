#pragma once

#include "core/DLLExport.h"

namespace symphony
{
	// Mother class
	SymphonyClass Texture
	{
	public:
		virtual ~Texture() = default;

		virtual void* GetTextureBuffer() = 0;
	};
}
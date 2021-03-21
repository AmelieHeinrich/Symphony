#pragma once

#include "core/PlatformDetection.h"
#include "core/DLLExport.h"
#include <memory>

namespace symphony {
	// Mother class (by the way I don't know  why the Render Surface has the renderer's function...
	// TODO : make the code more professionnal lmao
	SymphonyClass RenderSurface
	{
	public:
		virtual void Init(void *extraparams) {}
		virtual void Shutdown() {}
		virtual void Clear() {}
		virtual void ClearColor(float r, float g, float b, float a) {}
		virtual void ResizeBuffers(int x, int y, int w, int h) {}
	};
}
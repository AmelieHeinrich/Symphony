#pragma once

namespace symphony
{
	// Simple struct that wrapps stb_image functions
	struct ImageData
	{
		int Width;
		int Height;
		int Channels;
		int Dimensions;
		unsigned char* DataBuffer;
		float* FloatDataBuffer;

		static ImageData LoadImageData(const char* filepath, int dimensions);
		static ImageData LoadFloatImageData(const char* filepath, int dimensions);
		static void FreeImageData(ImageData data);
	};
}
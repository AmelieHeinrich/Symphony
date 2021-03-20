#pragma once

namespace symphony
{
	struct ImageData
	{
		int Width;
		int Height;
		int Channels;
		int Dimensions;
		unsigned char* DataBuffer;

		static ImageData LoadImageData(const char* filepath, int dimensions);
		static void FreeImageData(ImageData data);
	};
}
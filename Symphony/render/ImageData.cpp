#include "ImageData.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <stdexcept>

namespace symphony
{
	ImageData ImageData::LoadImageData(const char* filepath, int dimensions)
	{
		ImageData data{};
		
		data.DataBuffer = stbi_load(filepath, &data.Width, &data.Height, &data.Channels, STBI_rgb_alpha);

		if (!data.DataBuffer) {
			throw std::runtime_error("Failed to load image!");
		}

		data.Dimensions = dimensions;

		return data;
	}

	void ImageData::FreeImageData(ImageData data)
	{
		stbi_image_free(data.DataBuffer);
	}
}
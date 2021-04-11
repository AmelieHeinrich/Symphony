#include "ImageData.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <stdexcept>
#include "core/Application.h"

namespace symphony
{
	ImageData ImageData::LoadImageData(const char* filepath, int dimensions)
	{
		ImageData data{};
		
		ThreadPool& pool = Application::Get().GetThreadPool();
		std::future<uint8_t*> dataBuffer = pool.Submit(stbi_load, filepath, &data.Width, &data.Height, &data.Channels, STBI_rgb_alpha);
		data.DataBuffer = dataBuffer.get();

		if (!data.DataBuffer) {
			SY_CORE_ERROR("Failed to load image!");
		}

		data.Dimensions = dimensions;

		return data;
	}

	void ImageData::FreeImageData(ImageData data)
	{
		stbi_image_free(data.DataBuffer);
	}
}
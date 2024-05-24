#define STB_IMAGE_IMPLEMENTATION // Exactly one .CPP (this Image.cpp) should #define this before #including stb_image.h
#include "ThirdParty/stb/stb_image.h"
#include "Engine/Core/Image.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"

Image::Image()
{
}

Image::~Image()
{
}

Image::Image(const char* imageFilePath)
{
	m_imageFilePath = imageFilePath;
	int channels;
	stbi_set_flip_vertically_on_load(1);
	unsigned char* img = stbi_load(imageFilePath, &m_dimensions.x, &m_dimensions.y, &channels, 0);
	if (img == NULL) {
		//std::string msg = "Fail to load image";
			//imageFilePath;
		//msg.append(imageFilePath);
		ERROR_AND_DIE(Stringf("Failed to load image \"%s\"", imageFilePath));
	}
	for (int j = 0; j < m_dimensions.y; j++) {
		for (int i = 0; i < m_dimensions.x; i++) {
			int index = i + j * m_dimensions.x;
			Rgba8 tempColor;
		
			if (channels == 1) {
				tempColor = Rgba8(img[index * channels],
					img[index * channels],
					img[index * channels]);
			}
			else if (channels == 2) {
				tempColor = Rgba8(img[index * channels],
					img[index * channels + 1],
					img[index * channels + 1]);
			}
			else if (channels == 3) {
				tempColor = Rgba8(img[index * channels],
					img[index * channels + 1],
					img[index * channels + 2]);
			}
			else if (channels > 3) {
				tempColor = Rgba8(img[index * channels],
					img[index * channels + 1],
					img[index * channels + 2]);
				tempColor.a = img[index * channels + 3];
			}
			m_texelRgba8Data.push_back(tempColor);
		}
	}

	stbi_image_free(img);
}

Image::Image(IntVec2 size, Rgba8 color)
{
	m_dimensions = size;

	for (int j = 0; j < m_dimensions.y; j++) {
		for (int i = 0; i < m_dimensions.x; i++) {
			m_texelRgba8Data.push_back(color);
		}
	}
}

std::string const& Image::GetImageFilePath() const
{
	return m_imageFilePath;
}

IntVec2 Image::GetDimensions() const
{
	return m_dimensions;
}

const void* Image::GetRawData() const
{
	return m_texelRgba8Data.data();
}

Rgba8 Image::GetTexelColor(IntVec2 const& texelCoords) const
{
	int index = texelCoords.x + texelCoords.y * m_dimensions.x;
	return m_texelRgba8Data[index];
}

void Image::SetTexelColor(IntVec2 const& texelCoords, Rgba8 const& newColor)
{
	int index = texelCoords.x + texelCoords.y * m_dimensions.x;
	m_texelRgba8Data[index] = newColor;
}

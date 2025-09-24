#include "CubeMap.h"
#include "stb_image.h"

#include <iostream>

const Array<std::string> CubeMap::FaceNames = { "right", "left", "top", "bottom", "front", "back" };

CubeMap::CubeMap()
{
}

CubeMap::CubeMap(const std::string_view& InTextureLocation, const std::string_view& InType)
{
	TextureLocation = InTextureLocation;

	Type = InType;

	int width, height, nrChannels;

	glGenTextures(1, &ID);

	glBindTexture(GL_TEXTURE_CUBE_MAP, ID);

	for (unsigned int i = 0; i < FaceNames.GetSize(); i++)
	{
		unsigned char* data = LoadTexture(&width, &height, &nrChannels, FaceNames[i]);

		if (!data)
		{
#if DEBUG
			std::string texPathStr =  GetFullTexturePath(FaceNames[i]);
			std::cout << "Failed to load texture at address: " << texPathStr << std::endl;
#endif
			stbi_image_free(data);
			return;
		};

		GenerateByChannel(nrChannels, width, height, data, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i);

		stbi_image_free(data);
	}

	//TODO make it so you can pass this in to constructer
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

}

void CubeMap::GenerateByChannel(const std::uint8_t& nrChannels, const unsigned int& width, const unsigned int& height,
	const unsigned char* data, const GLenum& target) const
{
	if (nrChannels == 3)
	{
		glTexImage2D(target, 0, GL_SRGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	}
	else if (nrChannels == 4)
	{
		glTexImage2D(target, 0, GL_SRGB8_ALPHA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	}
}

std::string CubeMap::GetFullTexturePath(const std::string& FaceName) const
{
	return  std::string(PROJECT_ROOT_DIR) + "/" + TextureLocation + "/" + FaceName + ".jpg";
}

unsigned char* CubeMap::LoadTexture(int* Width, int* Height, int* nrChannels, const std::string& FaceName) const
{
	stbi_set_flip_vertically_on_load(false);

	const std::string texPathStr = GetFullTexturePath(FaceName);

	const char* address = texPathStr.c_str();

	unsigned char* data = stbi_load(address, Width, Height, nrChannels, 0);
	return data;
}

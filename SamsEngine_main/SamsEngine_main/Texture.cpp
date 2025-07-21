#include "Texture.h"
#include "stb_image.h"
#include <sstream>
#include <filesystem>
#include <glad/glad.h>
#include "GLFW/glfw3.h"

namespace fs = std::filesystem;

Texture::Texture()
{
}

Texture::Texture(const std::string InTextureLocation)
{
	TextureLocation = InTextureLocation;

	int width, height;

	unsigned char* data = LoadTexture(&width, &height);

	if (!data)
	{
		std::cout << "Failed to load texture" << std::endl;
		return;
	}

	glGenTextures(1, &ID);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, ID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(data);
}

unsigned char* Texture::LoadTexture(int* Width, int* Height) const
{
	stbi_set_flip_vertically_on_load(true);

	fs::path texPath = fs::absolute(TextureLocation);
	std::string texPathStr = texPath.string();
	const char* address = texPathStr.c_str();

	int nrChannels;
	unsigned char* data = stbi_load(address, Width, Height, &nrChannels, 0);
	return data;
}

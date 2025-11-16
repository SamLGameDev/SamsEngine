#include "Texture.h"
#include "stb_image.h"
#include <glad/glad.h>
#include <iostream>
#include "CorePaths.h"

std::function<BaseTexture* (const std::string_view& InTextureLocation, const TextureType& InType)> Texture::TextureCreationFunc;

const std::map<TextureType, GLint> Texture::ColorChannel =
{
	{diffuse, GL_SRGB8},
	{specular, GL_RGB8},
	{normal, GL_RGB8},
	{height, GL_RGB8},
};

const std::map<TextureType, GLint> Texture::ColorChannelWithAlpha = {
	{diffuse, GL_SRGB8_ALPHA8},
	{specular, GL_RGBA8},
	{normal, GL_RGBA8},
	{height, GL_RGBA8},
};

Texture::Texture()
{
}

Texture::Texture(const std::string_view& InTextureLocation, const TextureType& InType)
{
	RealTexture = TextureCreationFunc(InTextureLocation, InType);
}

Texture::~Texture()
{
	if (RealTexture != nullptr)
	{
		return;
	}

	delete RealTexture;
}

void Texture::GenerateByChannel(const std::uint8_t& nrChannels, const unsigned int& width, const unsigned int& height, const unsigned char* data) const
{
	if (nrChannels == 3)
	{
		const GLint colorChannel = ColorChannel.at(Type);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			colorChannel,
			width,
			height,
			0,
			GL_RGB,
			GL_UNSIGNED_BYTE,
			data);
	}
	else if (nrChannels == 4)
	{
		const GLint colorChannel = ColorChannelWithAlpha.at(Type);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			colorChannel,
			width,
			height,
			0,
			GL_RGBA,
			GL_UNSIGNED_BYTE,
			data);
	}
}

std::string Texture::GetFullTexturePath() const
{
	return  CorePaths::Contents.Path + "/" + TextureLocation;
}

unsigned char* Texture::LoadTexture(int* Width, int* Height, int* nrChannels) const
{
	stbi_set_flip_vertically_on_load(true);
	std::string texPathStr = GetFullTexturePath();
	const char* address = texPathStr.c_str();

	unsigned char* data = stbi_load(address, Width, Height, nrChannels, 0);

#if DEBUG
	if (data == nullptr)
	{
		std::cout << "ERROR::TEXTURE::FAILED TO LOAD TEXTURE AT: " << address << "\n";
	}
	else
	{
		std::cout << "INFO::TEXTURE::LOADED TEXTURE AT: " << address << "\n";
	}
#endif



	return data;
}

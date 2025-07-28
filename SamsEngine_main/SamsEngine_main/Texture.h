#pragma once
#include <iostream>
#include <string>

class Texture
{
public:

	Texture();

	Texture(const std::string InTextureLocation, std::string InType);

	Texture(const Texture& Other)
	{
		TextureLocation = Other.GetTextureLocation();
		ID = Other.GetID();
	}

	const unsigned int GetID() const
	{
		return ID;
	}

	std::string GetTextureLocation() const
	{
		return TextureLocation;
	}

	std::string GetType()
	{
		return Type;
	}

private:

	unsigned char* LoadTexture(int* Width, int* Height) const;

	std::string TextureLocation;

	unsigned int ID;

	std::string Type;

};


#pragma once
#include <iostream>
#include <string>

class Texture
{
public:

	Texture();

	Texture(const std::string InTextureLocation);

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

private:

	unsigned char* LoadTexture(int* Width, int* Height) const;

	std::string TextureLocation;

	unsigned int ID;

};


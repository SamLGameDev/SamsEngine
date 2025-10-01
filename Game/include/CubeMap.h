#pragma once

#include "Array.h"
#include <cstdint>
#include <glad/glad.h>
#include <string>

class CubeMap
{
public:

	CubeMap();

	/**
	 * loads the cube-map at that location.
	 * @param InTextureLocation starts from where the main file is located. i.e. Contents/Textures/texture
	 */
	CubeMap(const std::string_view& InTextureLocation, const std::string_view& InType);


	/**
	 * Generate a texture according to how may channels it has, 3 for rgb, 4 for rgba
	 */
	void GenerateByChannel(const std::uint8_t& nrChannels, const unsigned int& width, const unsigned int& height, const unsigned char* data, const GLenum& target) const;

	CubeMap(const CubeMap& Other)
	{
		//only need to copy the location, ID, and type, as its already beem generated and bound at creation

		TextureLocation = Other.GetTextureLocation();
		ID = Other.GetID();
		Type = Other.Type;
	}


	/**
	 * @return The buffer ID assigned to this cubemap
	 */
	[[nodiscard]] inline unsigned int GetID() const
	{
		return ID;
	}


	/**
	 * @return The Cubemap location, not the full path
	 */
	[[nodiscard]] inline std::string GetTextureLocation() const
	{
		return TextureLocation;
	}


	/**
 	 * @return The Cube-maps type, i.e. diffuse
 	 */
 	[[nodiscard]] inline std::string GetType() const
	{
		return Type;
	}


	/**
	 * @return Gets the full Path to the input faces texture
	 */
	[[nodiscard]] std::string GetFullTexturePath(const std::string& FaceName) const;

private:

	/**
	 * loads the specified texture from disk
	 */
	[[nodiscard]] unsigned char* LoadTexture(int* Width, int* Height, int* nrChannels, const std::string& FaceName) const;

	std::string TextureLocation;

	unsigned int ID;

	std::string Type;

	const static Array<std::string> FaceNames;
};

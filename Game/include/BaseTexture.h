// DO NOT MARK 
//This is because it has been submitted for my COMP305. Link to Original: https://github.falmouth.ac.uk/GA-Undergrad-Student-Work-25-26/Comp305-Engine-SL295211.git

#pragma once
#include <string>
#include <map>
#include <cstdint>

#include "glad/glad.h"


/**
 * Type of the texture, used to decided how its loaded, and what the shader does with it
 */
enum TextureType : std::int8_t
{
	diffuse,
	specular,
	normal,
	height,
};


class BaseTexture
{
public:

	BaseTexture() = default;
	virtual ~BaseTexture(){};

	BaseTexture(const BaseTexture& Other) = default;

	BaseTexture(BaseTexture&& Other) noexcept = default;

	BaseTexture& operator=(const BaseTexture& Other) = default;
	BaseTexture& operator=(BaseTexture&& Other) noexcept = default;

	/**
	 * loads the texture at that location. Srgb for diffuse, rgb for everything else
	 * @param InTextureLocation starts from where the main file is located. i.e. Contents/Textures/texture
	 */
	BaseTexture(const std::string_view& InTextureLocation, const TextureType& InType) {};


	/**
	 * @return The buffer ID assigned to this texture
	 */
	[[nodiscard]] virtual inline unsigned int GetID() const = 0;
		/**
		 * @return The texture location, not the full path
		 */
		[[nodiscard]] virtual  inline std::string GetTextureLocation() const = 0;

	/**
	 * @return Gets the full Path to the input faces texture
	 */
	[[nodiscard]] virtual std::string GetFullTexturePath()const=  0;

	/**
	* @return The textures type, i.e. diffuse
	*/
	[[nodiscard]] virtual inline TextureType GetType() const = 0;

protected:
	uint32_t ID;

private:

	/**
	 * loads the specified texture from disk
	 */
	[[nodiscard]] virtual unsigned char* LoadTexture(int* Width, int* Height, int* nrChannels) const = 0;

	std::string TextureLocation;

	TextureType Type;

	/**
	 * Map to give yoy the color channel to load the texture by based on type
	 */
	const static std::map<TextureType, GLint> ColorChannel;

	/**
	 * Map to give yoy the color channel to load the texture by based on type, but with alpha
	 */
	const static std::map<TextureType, GLint> ColorChannelWithAlpha;
};

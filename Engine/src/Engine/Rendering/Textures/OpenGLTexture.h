

#pragma once
#include <string>
#include <map>
#include <cstdint>
#include <memory>

#include "BaseTexture.h"

#include "glad/glad.h"

namespace OpenGL {



	class Texture : public BaseTexture
	{
	public:

		Texture();


		/**
		 * loads the texture at that location. Srgb for diffuse, rgb for everything else
		 * @param InTextureLocation starts from where the main file is located. i.e. Contents/Textures/texture
		 */
		Texture(const std::string_view& InTextureLocation, const TextureType& InType);

		/**
		 * Generate a texture according to how many channels it has, i.e. 3 for rgb, 4 for rgba
		 */
		void GenerateByChannel(const std::uint8_t& nrChannels, const unsigned int& width, const unsigned int& height, const unsigned char* data) const;

		inline Texture(const Texture& Other)
		{
			//only need to copy the location, ID, and type, as its already beem generated and bound at creation

			TextureLocation = Other.GetTextureLocation();
			ID = Other.GetID();
			Type = Other.Type;
		}

		/**
		 * @return The buffer ID assigned to this texture
		 */
		[[nodiscard]] inline unsigned int GetID() const
		{
			return ID;
		}

		/**
		 * @return The texture location, not the full path
		 */
		[[nodiscard]] inline std::string GetTextureLocation() const
		{
			return TextureLocation;
		}

		/**
		 * @return Gets the full Path to the input faces texture
		 */
		[[nodiscard]] std::string GetFullTexturePath()const;

		/**
		* @return The textures type, i.e. diffuse
		*/
		[[nodiscard]] inline TextureType GetType() const
		{
			return Type;
		}


		static std::shared_ptr<BaseTexture> CreateOpenGLTexture(const std::string_view& InTextureLocation, const TextureType& InType);

	private:

		/**
		 * loads the specified texture from disk
		 */
		[[nodiscard]] unsigned char* LoadTexture(int* Width, int* Height, int* nrChannels) const;

		std::string TextureLocation;

		unsigned int ID;

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
}

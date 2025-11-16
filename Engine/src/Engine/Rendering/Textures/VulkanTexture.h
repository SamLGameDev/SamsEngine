#pragma once
#include <string>
#include <map>
#include <cstdint>
#include <vulkan/vulkan_core.h>

#include "glad/glad.h"
#include "BaseTexture.h"

namespace Vulkan {


	class Texture : BaseTexture
	{
	public:

		Texture();
		~Texture()override = default;

		/**
		 * loads the texture at that location. Srgb for diffuse, rgb for everything else
		 * @param InTextureLocation starts from where the main file is located. i.e. Contents/Textures/texture
		 */
		Texture(const std::string_view& InTextureLocation, const TextureType& InType);

		/**
		 * Generate a texture according to how many channels it has, i.e. 3 for rgb, 4 for rgba
		 */
		void GenerateByChannel(const std::uint8_t& nrChannels, const unsigned int& width, const unsigned int& height, const unsigned char* data) const;

		inline Texture(const Texture& Other) : BaseTexture(Other)
		{
			//only need to copy the location, ID, and type, as its already beem generated and bound at creation

			TextureLocation = Other.GetTextureLocation();
			ID = Other.GetID();
			Type = Other.Type;
			
		}

		static BaseTexture* CreateVulkanTexture(const std::string_view& InTextureLocation, const TextureType& InType);

		/**
		 * @return The buffer ID assigned to this texture
		 */
		[[nodiscard]] inline unsigned int GetID() const override
		{
			return ID;
		}

		/**
		 * @return The texture location, not the full path
		 */
		[[nodiscard]] inline std::string GetTextureLocation() const override
		{
			return TextureLocation;
		}

		/**
		 * @return Gets the full Path to the input faces texture
		 */
		[[nodiscard]] std::string GetFullTexturePath()const override;

		/**
		* @return The textures type, i.e. diffuse
		*/
		[[nodiscard]] inline TextureType GetType() const override
		{
			return Type;
		}

	private:

		/**
		 * loads the specified texture from disk
		 */
		[[nodiscard]] unsigned char* LoadTexture(int* Width, int* Height, int* nrChannels) const override;

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
}

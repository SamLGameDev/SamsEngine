#pragma once

#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Array.h"
#include "LinkedList.h"
#include "Texture.h"
#include "Vector3D.h"
#include "CubeMap.h"
#include "BaseShader.h"
#include <functional>
#include <memory>

class Shader
{
public:

	Shader();
	~Shader();

	/**
	 * loads the shader at that location. It loads all parts of the shader, so vert, geom, and frag
	 * @param InName The name of the shader to load
	 * @param InStorageLocation starts from where the main file is located. i.e. Contents/Shader/
	 */
	Shader(const std::string_view& InName, const std::string_view& InStorageLocation);

	Shader(const Shader& Other)
	{
		RealShader = Other.RealShader;
	}

	Shader& operator=(const Shader& Other) {
		if (this != &Other) { // prevent self-assignment

			RealShader = Other.RealShader;
		}
		return *this;
	}

	void Move(Shader& Other)
	{
		RealShader = std::move(Other.RealShader);
	}

	Shader(Shader&& Other) noexcept
	{
		Move(Other);
	}

	Shader& operator=(Shader&& Other) noexcept
	{
		if (this != &Other) {
			Move(Other);
		}
		return *this;
	}

	/**
	 * Activates the shader to be applied to draw calls
	 */
	void Use() const;

	void SetUniformBuffer(const size_t& Location, const void* Data, const size_t& Size);

	/**
	 * Apply all the textures for the next set of rendered objects
	 */
	void ApplyTextures() const;

	void AddTexture(const Texture InTexture);
	void AddTexture(const Array<Texture>& InTexture);


	/**
	 * @return The folder containing the shaders
	 */
	[[nodiscard]] std::string GetRawStorageLocation() const
	{
		return RealShader->GetRawStorageLocation();
	}

	[[nodiscard]] std::string GetName() const
	{
		return RealShader->GetName();
	}

	/**
	 * @return The assigned buffer of the shader program
	 */
	[[nodiscard]] unsigned int GetID() const
	{
		return RealShader->GetID();
	}

	[[nodiscard]] Array<Texture> GetTextures() const
	{
		return RealShader->GetTextures();
	}

	void AddCubeMap(const CubeMap& InMap);


	static std::function<std::shared_ptr<BaseShader> (const std::string_view& InName, const std::string_view& InStorageLocation)> ShaderCreationFunc;

private:


	std::string GetPathUntyped() const;


	/**
	 * @return The full path to the vertex shader
	 */
	[[nodiscard]] std::string GetShaderLocation() const;

	/**
	 * @return The full path to the geometry shader
	 */
	[[nodiscard]] std::string GetGeometryLocation() const;

	/**
	 * @return The full path to the fragment shader
	 */
	[[nodiscard]] std::string GetFragmentLocation() const;


	std::shared_ptr<BaseShader> RealShader;

};

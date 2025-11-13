#pragma once

#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Array.h"
#include "LinkedList.h"
#include "Texture.h"
#include "Vector3D.h"
#include "CubeMap.h"

class BaseShader
{
public:

	BaseShader();
	virtual ~BaseShader(){};

	/**
	 * loads the shader at that location. It loads all parts of the shader, so vert, geom, and frag
	 * @param InName The name of the shader to load
	 * @param InStorageLocation starts from where the main file is located. i.e. Contents/Shader/
	 */
	BaseShader(const std::string_view& InName, const std::string_view& InStorageLocation){};

	BaseShader(const BaseShader& Other)
	{
		StorageLocation = Other.GetRawStorageLocation();
		Name = Other.GetName();
		ID = Other.GetID();
		Textures = Other.Textures;
		Map = Other.Map;
	}

	BaseShader& operator=(const BaseShader& Other) {
		if (this != &Other) { // prevent self-assignment
			StorageLocation = Other.StorageLocation;
			Name = Other.Name;
			ID = Other.ID;
			Textures = Other.Textures;
			Map = Other.Map;
		}
		return *this;
	}


	/**
	 * Activates the shader to be applied to draw calls
	 */
	virtual void Use() = 0;

	virtual void SetUniformBuffer(const size_t& Location, const void* Data, const size_t& Size) = 0;
	/**
	 * Sets the shaders uniform float value
	 */
	virtual void SetFloat(const std::string_view& InName, const float& Value) = 0;

	/**
	 * Sets the shaders uniform int value
	 */
	virtual void SetInt(const std::string_view& InName, const int& Value) const = 0;

	/**
	 * Sets the shaders uniform mat4 value
	 */
	virtual void SetMatrix4fv(const std::string_view& InName, const GLfloat* Value) const = 0;

	/**
	 * Sets the shaders uniform mat3 value
	 */
	virtual void SetMatrix3fv(const std::string_view& InName, const GLfloat* Value) const = 0;

	/**
	 * Sets the shaders uniform vec4 value
	 */
	virtual void SetVec4(const std::string_view& InName, const Array<float>& Value) const = 0;

	/**
	 * Sets the shaders uniform vec3 value
	 */
	virtual void SetVec3(const std::string_view& InName, const Array<float>& Value) const = 0;

	/**
	 * Sets the shaders uniform vec3 value
	 */
	virtual void SetVec3(const std::string_view& InName, const Vector3D& Value) const = 0;


	/**
	 * Apply all the textures for the next set of rendered objects
	 */
	virtual void ApplyTextures() const = 0;

	virtual void AddTexture(const Texture InTexture) = 0;
	virtual void AddTexture(const Array<Texture>& InTexture) = 0;


	/**
	 * @return The folder containing the shaders
	 */
	virtual [[nodiscard]] std::string GetRawStorageLocation() const
	{
		return StorageLocation;
	}

	virtual [[nodiscard]] std::string GetName() const
	{
		return Name;
	}

	/**
	 * @return The assigned buffer of the shader program
	 */
	virtual [[nodiscard]] unsigned int GetID() const
	{
		return ID;
	}

	virtual [[nodiscard]] LinkedList<Texture> GetTextures() const
	{
		return Textures;
	}

	virtual void AddCubeMap(const CubeMap& InMap) = 0;

private:

	//TODO update all the default files to reflect new system
	/**
	 * Creates a default vertex file for rendering an object
	 * @return true if file successfully created
	 */
	virtual bool CreateDefaultShaderFile() const = 0;

	/**
	 * Creates a default geometry file for rendering an object
	 * @return true if file successfully created
	 */
	virtual bool CreateDefaultGeometryFile() const = 0;

	/**
	 * Creates a default fragment file for rendering an object
	 * @return true if file successfully created
	 */
	virtual bool CreateDefaultFragmentFile() const = 0;


	/**
	 * @return true if the vertex file exists
	 */
	virtual [[nodiscard]] bool DoesVertexShaderExist() const = 0;

	/**
	 * @return true if the geometry file exists
	 */
	virtual [[nodiscard]] bool DoesGeometryShaderExist() const = 0;

	/**
	 * @return true if the fragment file exists
	 */
	virtual [[nodiscard]] bool DoesFragmentShaderExist() const = 0;

	virtual std::string GetPathUntyped() const = 0;


	/**
	 * @return The full path to the vertex shader
	 */
	virtual [[nodiscard]] std::string GetShaderLocation() const = 0;

	/**
	 * @return The full path to the geometry shader
	 */
	virtual [[nodiscard]] std::string GetGeometryLocation() const = 0;

	/**
	 * @return The full path to the fragment shader
	 */
	virtual [[nodiscard]] std::string GetFragmentLocation() const = 0;


	virtual void CreateProgram(const unsigned int& vertex, const unsigned int& fragment, const unsigned int& geometry) = 0;

	virtual [[nodiscard]] std::string ReadFileContents(const std::string_view& Location) const = 0;

	std::string StorageLocation;

	std::string Name;

	unsigned int ID;

	LinkedList<Texture> Textures;

	CubeMap Map;
};

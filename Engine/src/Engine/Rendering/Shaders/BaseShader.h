// DO NOT MARK 
//This is because it has been submitted for my COMP305. Link to Original: https://github.falmouth.ac.uk/GA-Undergrad-Student-Work-25-26/Comp305-Engine-SL295211.git
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

	BaseShader(const BaseShader& Other) = default;

	BaseShader& operator=(const BaseShader& Other) = default;


	/**
	 * Activates the shader to be applied to draw calls
	 */
	virtual void Use() = 0;

	virtual void SetUniformBuffer(const size_t& Location, const void* Data, const size_t& Size) = 0;


	virtual void AddTexture(const Texture InTexture) = 0;
	virtual void AddTexture(const Array<Texture>& InTexture) = 0;


	/**
	 * @return The folder containing the shaders
	 */
	virtual [[nodiscard]] std::string GetRawStorageLocation() const = 0;

	virtual [[nodiscard]] std::string GetName() const = 0;
	/**
	 * @return The assigned buffer of the shader program
	 */
	virtual [[nodiscard]] unsigned int GetID() const = 0;

	virtual [[nodiscard]] Array<Texture> GetTextures() const = 0;

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

	virtual [[nodiscard]] std::string ReadFileContents(const std::string_view& Location) const = 0;

};

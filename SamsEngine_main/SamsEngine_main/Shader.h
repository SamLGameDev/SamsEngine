#pragma once

#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Array.h"
#include "LinkedList.h"
#include "Texture.h"


class Shader
{

public:

	Shader();

	Shader(std::string InName, std::string InStorageLocation);

	void Use() const;

	void SetFloat(const std::string InName, float Value) const;

	void SetInt(const std::string InName, int Value) const;

	void SetMatrix4fv(const std::string InName, GLfloat* Value) const;


	void Set4Float(const std::string InName, Array<float> Value) const;

	void ApplyTextures();

	void AddTexture(Texture InTexture);


private:


	const bool CreateDefaultShaderFile() const;
	void CreateDefaultFragmentFile() const;

	const bool DoesShaderExist() const;

	const std::string GetShaderLocation() const;
	const std::string GetFragmentLocation() const;

	const unsigned int CompileVertex() const;

	const unsigned int CompileFragment() const;

	void CreateProgram(const unsigned int vertex, const unsigned int fragment);

	const std::string ReadFileContents(const std::string Location) const;


	std::string StorageLocation;

	std::string Name;

	unsigned int ID;

	LinkedList<Texture> Textures;

};


#pragma once
#include "Shader.h"
#include <iostream>
#include <fstream>
#include <sstream>

Shader::Shader(std::string InName, std::string InStorageLocation)
{
	StorageLocation = InStorageLocation;

	Name = InName;

	if (!DoesShaderExist())
	{ 
		bool bIsSuccessful = CreateDefaultShaderFile();
		if (!bIsSuccessful)
		{
			return;
		};

		CreateDefaultFragmentFile();
	}

	unsigned int vertex = CompileVertex();

	if (vertex == -1)
	{
		return;
	}

	unsigned int fragment = CompileFragment();

	if (fragment == -1)
	{
		return;
	}

	CreateProgram(vertex, fragment);



}

void Shader::Use() const
{
	glUseProgram(ID);
}

void Shader::SetFloat(const std::string InName, float Value) const
{
	glUniform1f(glGetUniformLocation(ID, InName.c_str()), Value);
}

void Shader::SetInt(const std::string InName, int Value) const
{
	glUniform1i(glGetUniformLocation(ID, InName.c_str()), Value);
}

void Shader::SetMatrix4fv(const std::string InName, GLfloat* Value) const
{
	glUniformMatrix4fv(glGetUniformLocation(ID, InName.c_str()), 1, GL_FALSE, Value);
}

void Shader::Set4Float(const std::string InName, Array<float> Value) const
{
	glUniform4f(glGetUniformLocation(ID, InName.c_str()), Value[0], Value[1], Value[2], Value[3]);
}

void Shader::CreateDefaultFragmentFile() const
{
	std::ofstream FragmentFile;

	FragmentFile.open(GetFragmentLocation());

	FragmentFile << "#version 330 core\n"
		"out vec4 FragColor;\n"

		"in vec3 VertexColor;\n"

		"in vec2 TexCoord; \n"

		"uniform sampler2D ourTexture; \n"
		"uniform float Visibility; \n"


		"void main()\n"
		"{\n"
		"   FragColor = texture(ourTexture, TexCoord) * vec4(VertexColor, Visibility);\n"
		"}\0";

	FragmentFile.close();
}

const bool Shader::CreateDefaultShaderFile() const
{
	std::ofstream ShaderFile;

	ShaderFile.open(GetShaderLocation());

	if (!ShaderFile.is_open())
	{
		std::cout << "ERROR::SHADER::COULD NOT CREATE SHADER AT LOCATION" << std::endl;
		return false;
	}

	ShaderFile << "#version 330 core\n"
		"layout (location = 0) in vec3 aPos;\n"
		"layout (location = 1) in vec3 aColor;\n"
		"layout (location = 2) in vec2 aTexture;\n"

		"out vec3 VertexColor;\n"

		"out vec4 Pos; \n"

		"out vec2 TexCoord; \n"

		"uniform float offset; \n"

		"uniform mat4 Transform; \n"
		"uniform mat4 Model; \n"
		"uniform mat4 View; \n"
		"uniform mat4 Projection; \n"

		"void main()\n"
		"{\n"
		"   Pos = Projection * View * Model * Transform * vec4(aPos, 1.0);\n"
		"   gl_Position = Pos;\n" //can use negative to reverse
		"	VertexColor = aColor + offset;\n"
		"	TexCoord = aTexture;\n"
		"}\0";

	ShaderFile.close();
	return true;
}

const bool Shader::DoesShaderExist() const
{
	struct stat Buffer;

	bool bDoesFileExist = (stat(GetShaderLocation().c_str(), &Buffer) == 0);

	return bDoesFileExist;
}

const std::string Shader::GetShaderLocation() const
{
	return StorageLocation + Name + ".sf";
}

const std::string Shader::GetFragmentLocation() const
{
	return StorageLocation + Name + ".ff";
}

const unsigned int Shader::CompileVertex() const
{

	int  success;
	char infoLog[512];



	std::string vertexCodeString = ReadFileContents(GetShaderLocation());

	const char* vertexCode = vertexCodeString.c_str();

	unsigned int vertex;

	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vertexCode, NULL);
	glCompileShader(vertex);

	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);

	if (!success)
	{
		glGetShaderInfoLog(vertex, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
		return -1;
	}

	return vertex;
}

const unsigned int Shader::CompileFragment() const
{

	int  success;
	char infoLog[512];

	std::string fragmentCodeString = ReadFileContents(GetFragmentLocation());

	const char* fragmentCode = fragmentCodeString.c_str();

	unsigned int fragment;

	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fragmentCode, NULL);
	glCompileShader(fragment);
	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);

	if (!success)
	{
		glGetShaderInfoLog(fragment, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
		return -1;
	}

	return fragment;
}

void Shader::CreateProgram(const unsigned int vertex, const unsigned int fragment)
{
	int  success;
	char infoLog[512];

	ID = glCreateProgram();
	glAttachShader(ID, vertex);
	glAttachShader(ID, fragment);
	glLinkProgram(ID);
	glGetProgramiv(ID, GL_LINK_STATUS, &success);

	if (!success) {
		glGetProgramInfoLog(ID, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

}

const std::string Shader::ReadFileContents(const std::string Location) const
{
	std::ifstream File;
	File.open(Location);
	std::stringstream Buffer;
	Buffer << File.rdbuf();
	File.close();

	std::string text = Buffer.str();

	return text;
}

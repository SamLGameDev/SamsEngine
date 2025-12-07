#pragma once
#include "Shader.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include "CorePaths.h"


std::function<std::shared_ptr<BaseShader>(const std::string_view& InName, const std::string_view& InStorageLocation)> Shader::ShaderCreationFunc;

Shader::Shader()
{
}

Shader::~Shader()
{
}

Shader::Shader(const std::string_view& InName, const std::string_view& InStorageLocation)
{
	RealShader = ShaderCreationFunc(InName, InStorageLocation);
}

void Shader::Use() const
{
	RealShader->Use();
}

void Shader::SetUniformBuffer(const size_t& Location, const void* Data, const size_t& Size)
{
	RealShader->SetUniformBuffer(Location, Data, Size);
}

void Shader::SetFloat(const std::string_view& InName, const float& Value) const
{
	RealShader->SetFloat(InName, Value);
}

void Shader::SetInt(const std::string_view& InName, const int& Value) const
{
	RealShader->SetInt(InName, Value);
}

void Shader::SetMatrix4fv(const std::string_view& InName, const GLfloat* Value) const
{
	RealShader->SetMatrix4fv(InName, Value);
}

void Shader::SetMatrix3fv(const std::string_view& InName, const GLfloat* Value) const
{
	RealShader->SetMatrix3fv(InName, Value);
}

void Shader::SetVec4(const std::string_view& InName, const Array<float>& Value) const
{
	RealShader->SetVec4(InName, Value);
}

void Shader::SetVec3(const std::string_view& InName, const Array<float>& Value) const
{
	RealShader->SetVec3(InName, Value);
}

void Shader::SetVec3(const std::string_view& InName, const Vector3D& Value) const
{
	RealShader->SetVec3(InName, Value);
}

void Shader::ApplyTextures() const
{
	RealShader->ApplyTextures();
}

void Shader::AddTexture(const Texture InTexture)
{
	RealShader->AddTexture(InTexture);
}

void Shader::AddTexture(const Array<Texture>& InTexture)
{
	RealShader->AddTexture(InTexture);
}

bool Shader::CreateDefaultFragmentFile() const
{
	std::ofstream FragmentFile;

	FragmentFile.open(GetFragmentLocation().data());

	if (!FragmentFile.is_open())
	{
#if DEBUG
		std::cout << "ERROR::FRAGMENT*::COULD NOT CREATE SHADER AT LOCATION" << std::endl;
#endif
		return false;
	}

	FragmentFile << "#version 330 core\n"
		"out vec4 FragColor;\n"

		"in vec3 VertexColor;\n"

		"in vec2 TexCoord; \n"

		"uniform sampler2D ourTexture0; \n"
		"uniform float Visibility; \n"

		"void main()\n"
		"{\n"
		"   FragColor = texture(ourTexture0, TexCoord) * vec4(VertexColor, Visibility);\n"
		"}\0";

	FragmentFile.close();
	return true;
}

void Shader::AddCubeMap(const CubeMap& InMap)
{
	RealShader->AddCubeMap(InMap);
}

bool Shader::CreateDefaultShaderFile() const
{
	std::ofstream ShaderFile;

	ShaderFile.open(GetShaderLocation().data());

	if (!ShaderFile.is_open())
	{
#if DEBUG
		std::cout << "ERROR::SHADER::COULD NOT CREATE SHADER AT LOCATION::" << GetShaderLocation()<< std::endl;
#endif
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

bool Shader::CreateDefaultGeometryFile() const
{
	std::ofstream GeometryFile;

	GeometryFile.open(GetGeometryLocation().data());

	if (!GeometryFile.is_open())
	{
#if DEBUG
		std::cout << "ERROR::SHADER::GEOMETRY::COULD NOT CREATE SHADER AT LOCATION" << std::endl;
#endif
		return false;
	}

	GeometryFile << "#version 460 core\n"
		"layout (triangles) in;\n"
		"layout (triangle_strip, max_vertices = 3) out;\n"

		"void main()\n"
		"{\n"
		"   for (int i = 0; i < 3; i++)\n"
		"   {\n"
		"        gl_Position = gl_in[i].gl_Position;\n" //can use negative to reverse
		"	     EmitVertex();\n"
		"	};\n"
		"   EndPrimitive();\n"
		"}\0";

	GeometryFile.close();
	return true;
}

bool Shader::DoesVertexShaderExist() const
{
	struct stat Buffer;

	const bool bDoesFileExist = stat(GetShaderLocation().data(), &Buffer) == 0;

	return bDoesFileExist;
}

bool Shader::DoesGeometryShaderExist() const
{
	struct stat Buffer;

	const bool bDoesFileExist = stat(GetGeometryLocation().data(), &Buffer) == 0;

	return bDoesFileExist;
}

bool Shader::DoesFragmentShaderExist() const
{
	struct stat Buffer;

	const bool bDoesFileExist = stat(GetFragmentLocation().data(), &Buffer) == 0;

	return bDoesFileExist;
}

std::string Shader::GetPathUntyped() const
{
	return RealShader->GetRawStorageLocation();
}

std::string Shader::GetShaderLocation() const
{
	return GetPathUntyped() + ".vert";
}

std::string Shader::GetGeometryLocation() const
{
	return GetPathUntyped() + ".geom";
}

std::string Shader::GetFragmentLocation() const
{
	return GetPathUntyped() + ".frag";
}

unsigned int Shader::CompileVertex() const
{
	return -1;
}

unsigned int Shader::CompileGeometry() const
{


	return -1;
}

unsigned int Shader::CompileFragment() const
{


	return -1;
}

void Shader::CreateProgram(const unsigned int& vertex, const unsigned int& fragment, const unsigned int& geometry)
{

}

std::string Shader::ReadFileContents(const std::string_view& Location) const
{
	std::ifstream File;
	File.open(Location.data());
	std::stringstream Buffer;
	Buffer << File.rdbuf();
	File.close();

	const std::string text = Buffer.str();

	return text;
}

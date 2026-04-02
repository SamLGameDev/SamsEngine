


#pragma once
#include "OpenGLShader.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include "CorePaths.h"
#include "DataBuffers.h"
#include "DataBuffersOpenGL.h"
#include "Transform.h"
#include "Voronoi.h"

namespace OpenGL {

	Shader::Shader()
	{
	}

	Shader::Shader(const std::string_view& InName, const std::string_view& InStorageLocation)
	{

		GLenum error = glGetError();
		if (error != GL_NO_ERROR)
		{
			std::cout << "ERROR::UNIFORMBUFFER::" << std::to_string(error) << std::endl;
		}
		StorageLocation = InStorageLocation;

		Name = InName;

		if (!DoesVertexShaderExist())
		{
			const bool bIsSuccessful = CreateDefaultShaderFile();
			if (!bIsSuccessful)
			{
#if DEBUG
				std::cout << "ERROR::SHADER::VERTEX::SHADER NOT CREATED" << '\n';
#endif
				return;
			}
		}

		const unsigned int vertex = CompileVertex();

		assert(vertex != -1);

		if (!DoesGeometryShaderExist())
		{
			const bool bIsSuccessful = CreateDefaultGeometryFile();
			if (!bIsSuccessful)
			{
#if DEBUG
				std::cout << "ERROR::SHADER::GEOMETRY::SHADER NOT CREATED" << '\n';
#endif
				return;
			};
		}

		const unsigned int geometry = CompileGeometry();

		assert(geometry != -1);

		if (!DoesFragmentShaderExist())
		{
			const bool bIsSuccessful = CreateDefaultFragmentFile();
			if (!bIsSuccessful)
			{
#if DEBUG
				std::cout << "ERROR::SHADER::FRAGMENT::SHADER NOT CREATED" << '\n';
#endif
				return;
			};
		}

		const unsigned int fragment = CompileFragment();

		assert(fragment != -1);

		CreateProgram(vertex, fragment, geometry);

		GLuint GlobalSize = sizeof(GlobalTransforms);

		GLuint LocalSize = sizeof(PerInstanceTransforms);

		Array<GLuint> sizes = { GlobalSize, LocalSize };

		size_t index = 0;

		for (const auto& size : sizes)
		{
			uint32_t id;
			::DataBuffers::GenBuffer(id);

			UniformBufferID.Add(id);
			UniformMappedData.Add(::DataBuffers::GenerateUniformDataBuffer(id, size));
		}

	}

	void Shader::ApplyTextures() const
	{
		unsigned int SpecularNum = 1;
		unsigned int DiffuseNum = 1;
		unsigned int HeightNum = 1;
		unsigned int NormalNum = 1;

		//assign the texture based on its type

		for (unsigned int i = 0; i < Textures.GetSize(); i++)
		{
			glActiveTexture(GL_TEXTURE0 + i);

			std::string number;
			std::string Type;

			if (Textures[i].GetType() == diffuse)
			{
				Type = "texture_diffuse";
				number = std::to_string(DiffuseNum++);
			}
			else if (Textures[i].GetType() == specular)
			{
				Type = "texture_specular";
				number = std::to_string(SpecularNum++);
			}
			else if (Textures[i].GetType() == height)
			{
				Type = "texture_height";
				number = std::to_string(HeightNum++);
			}
			else if (Textures[i].GetType() == normal)
			{
				Type = "texture_normal";
				number = std::to_string(NormalNum++);
			}

			//TODO Fix This by converting the enum to string, and update the shader file to reflect this. Also figure out why this works?
			std::string TextureSlot = "material." + Type + number;
		//	SetInt(TextureSlot, i);

			glBindTexture(GL_TEXTURE_2D, Textures[i].GetID());
		}

		//if there is a cube-map, apply it
		if (Map.GetTextureLocation() != "")
		{
			glActiveTexture(GL_TEXTURE0 + static_cast<GLuint>(Textures.GetSize()));

		//	SetInt("Map", Textures.GetSize());

			glBindTexture(GL_TEXTURE_CUBE_MAP, Map.GetID());
		}
	}

	void Shader::Use()
	{
		glUseProgram(ID);

		if (Textures.IsEmpty()) return;

		SetInt("texSampler", 0);
		glBindTexture(GL_TEXTURE_2D, Textures[0].GetID());
		glActiveTexture(GL_TEXTURE0);
	}

	void Shader::SetUniformBuffer(const size_t& Location, const void* Data, const size_t& Size)
	{

		DataBuffer* buffer = dynamic_cast<DataBuffer*>(::DataBuffers::GetBuffer(static_cast<uint32_t>(UniformBufferID[Location])));

		glBindBufferBase(GL_UNIFORM_BUFFER, static_cast<GLuint>(Location), buffer->UBO);

		memcpy(UniformMappedData[Location], Data, Size);
	}
	

	void Shader::AddTexture(const Texture InTexture)
	{
		Textures.Add(InTexture);
	}

	void Shader::AddTexture(const Array<Texture>& InTexture)
	{
		Textures.Add(InTexture);
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
		Map = InMap;
	}
	void Shader::SetInt(const std::string_view& InName, const int& Value) const
	{
		glUniform1i(glGetUniformLocation(ID, InName.data()), Value);
	}

	std::shared_ptr<BaseShader> Shader::CreateOpenGLShader(const std::string_view& InName,
		const std::string_view& InStorageLocation)
	{
		std::shared_ptr<Shader> shader = std::make_shared<Shader>(InName, InStorageLocation);

		return shader;
	}

	bool Shader::CreateDefaultShaderFile() const
	{
		std::ofstream ShaderFile;

		ShaderFile.open(GetShaderLocation().data());

		if (!ShaderFile.is_open())
		{
#if DEBUG
			std::cout << "ERROR::SHADER::COULD NOT CREATE SHADER AT LOCATION::" << GetShaderLocation() << std::endl;
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
		return CorePaths::Contents.Path + "/" + StorageLocation + Name;
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
		int  success;

		const std::string vertexCodeString = ReadFileContents(GetShaderLocation());

		const char* vertexCode = vertexCodeString.c_str();

		const unsigned int vertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex, 1, &vertexCode, NULL);
		glCompileShader(vertex);

		glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);

		if (!success)
		{
#if DEBUG
			char infoLog[512];
			glGetShaderInfoLog(vertex, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::VERTEX::" << Name << "::COMPILATION_FAILED\n" << infoLog << std::endl;
#endif
			return -1;
		}

		return vertex;
	}

	unsigned int Shader::CompileGeometry() const
	{
		int  success;

		const std::string geometryCodeString = ReadFileContents(GetGeometryLocation());

		const char* geometryCode = geometryCodeString.c_str();

		const unsigned int geometry = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(geometry, 1, &geometryCode, NULL);
		glCompileShader(geometry);

		glGetShaderiv(geometry, GL_COMPILE_STATUS, &success);

		if (!success)
		{
#if DEBUG
			char infoLog[512];
			glGetShaderInfoLog(geometry, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::GEOMETRY::" << Name << "::COMPILATION_FAILED\n" << infoLog << std::endl;
#endif
			return -1;
		}

		return geometry;
	}

	unsigned int Shader::CompileFragment() const
	{
		int  success;

		const std::string fragmentCodeString = ReadFileContents(GetFragmentLocation());

		const char* fragmentCode = fragmentCodeString.data();

		const unsigned int fragment = glCreateShader(GL_FRAGMENT_SHADER);

		glShaderSource(fragment, 1, &fragmentCode, NULL);
		glCompileShader(fragment);
		glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);

		if (!success)
		{
#if DEBUG
			char infoLog[512];
			glGetShaderInfoLog(fragment, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::FRAGMENT::" << Name << "::COMPILATION_FAILED\n" << infoLog << std::endl;
#endif
			return -1;
		}

		return fragment;
	}

	void Shader::CreateProgram(const unsigned int& vertex, const unsigned int& fragment, const unsigned int& geometry)
	{
		int  success;

		ID = glCreateProgram();
		glAttachShader(ID, vertex);
		glAttachShader(ID, geometry);
		glAttachShader(ID, fragment);
		glLinkProgram(ID);
		glGetProgramiv(ID, GL_LINK_STATUS, &success);

#if DEBUG

		if (!success) {

			char infoLog[512];
			glGetProgramInfoLog(ID, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::PROGRAM::COMPILATION_FAILED\n" << infoLog << std::endl;
		}
#endif
		glDeleteShader(vertex);
		glDeleteShader(geometry);
		glDeleteShader(fragment);
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
}

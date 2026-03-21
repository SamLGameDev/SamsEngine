// DO NOT MARK 
//This is because it has been submitted for my COMP305. Link to Original: https://github.falmouth.ac.uk/GA-Undergrad-Student-Work-25-26/Comp305-Engine-SL295211.git
#pragma once

#include <optional>

#include "Array.h"
#include "Verticie.h"
#include "Shader.h"
#include "Transform.h"
#include <utility>

class Mesh
{
public:
	Mesh();

	~Mesh();
	void Copy(const Mesh& Copy);

	Mesh(const Mesh& copy);


	Mesh& operator=(const Mesh& other)
	{
		if (this != &other)
		{
			Copy(other);
		}
		return *this;
	}

	void Move(Mesh& other)
	{
		Vertices = std::move(other.Vertices);
		Indices = std::move(other.Indices);
		FVerts = std::move(other.FVerts);
		FTexCoords = std::move(other.FTexCoords);
		MeshShader = std::move(other.MeshShader);
		VAO = other.VAO;
		Faces = std::move(other.Faces);
		other.VAO = 0;
	}

	Mesh(Mesh&& other) noexcept
	{
		Move(other);
	}

	Mesh& operator=(Mesh&& other) noexcept
	{
		if (this != &other)
		{
			Move(other);
		}
		return *this;
	}

	void Initialise();

	void Draw(const Transform* ModelTransform);

	void Draw(const Transform* ModelTransform, Shader& InShader) const;

	void RegenerateMesh();


	Array<Vertex> Vertices;

	Array<uint16_t> Indices;

	Array<Vector3D> FVerts;
	Array<Vector2D> FTexCoords;

	Array<Face> Faces;

	Shader MeshShader;

	uint32_t VAO;
private:

	void SetUpMesh();

	static void SetShaderVariables(const Transform* ModelTransform, Shader InShader);

};

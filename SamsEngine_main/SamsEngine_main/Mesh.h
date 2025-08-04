#pragma once

#include "Array.h"
#include "Verticie.h"
#include "Shader.h"
class Mesh
{

public:
	Mesh();

	Mesh(Array<Vertex> InVertices, Array<unsigned int> InIndices, Shader InShader);

	Mesh(const Mesh& Copy);

	void Draw();

private:

	Array<Vertex> Vertices;

	Array<unsigned int> Indices;

	Shader MeshShader;

	unsigned int VAO, VBO, EBO;

	void SetUpMesh();

};


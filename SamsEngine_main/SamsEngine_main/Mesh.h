#pragma once

#include "Array.h"
#include "Verticie.h"
#include "Shader.h"
#include "Transform.h"

class Mesh
{

public:
	Mesh();

	Mesh(Array<Vertex> InVertices, Array<unsigned int> InIndices, Shader InShader);

	Mesh(const Mesh& Copy);

	void Draw(const Transform* ModelTransform);

	void SetShaderVariables(const Transform* ModelTransform);

	void SetLightVariables(glm::mat4& view);

	void SetTransformationVariables(glm::mat4& model, glm::mat4& view, glm::mat4& projection, glm::mat3& normalModel);

	void RegenerateMesh();

	Array<Vertex> Vertices;

	Array<unsigned int> Indices;

	Shader MeshShader;

private:

	unsigned int VAO, VBO, EBO;

	void SetUpMesh();

};


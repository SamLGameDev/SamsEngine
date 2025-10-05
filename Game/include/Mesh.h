#pragma once

#include "Array.h"
#include "Verticie.h"
#include "Shader.h"
#include "Transform.h"

class Mesh
{
public:
	Mesh();

	Mesh(const Array<Vertex>& InVertices, const Array<unsigned int>& InIndices, const Shader& InShader);

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


	void Draw(const Transform* ModelTransform) const;

	void Draw(const Transform* ModelTransform, const Shader* InShader) const;

	void RegenerateMesh();


	Array<Vertex> Vertices;

	Array<unsigned int> Indices;

	Shader MeshShader;

	GLuint VAO, VBO, EBO;

	unsigned int* Instances;

private:

	void SetUpMesh();

	static void SetShaderVariables(const Transform* ModelTransform, const Shader* InShader);

	static void SetLightVariables(const glm::mat4& view, const Shader* InShader);

	static void SetTransformationVariables(const glm::mat4& model, const glm::mat3& normalModel, const Shader* InShader);
};

#pragma once

#include <optional>

#include "Array.h"
#include "Verticie.h"
#include "Shader.h"
#include "Transform.h"

class Mesh
{
public:
	Mesh();

	Mesh(const Array<Vertex>& InVertices, const Array<uint16_t>& InIndices, Shader& InShader);

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

	void Initialise();

	void Draw(const Transform* ModelTransform);

	void Draw(const Transform* ModelTransform, Shader* InShader) const;

	void RegenerateMesh();


	Array<Vertex> Vertices;

	Array<uint16_t> Indices;

	Array<Vector3D> FVerts;
	Array<Vector2D> FTexCoords;

	std::optional<Shader> MeshShader;

	GLuint VAO, VBO, EBO;

	unsigned int* Instances;

private:

	void SetUpMesh();

	static void SetShaderVariables(const Transform* ModelTransform, Shader* InShader);

	static void SetLightVariables(const glm::mat4& view, const Shader* InShader);

	static void SetTransformationVariables(const glm::mat4& model, const glm::mat3& normalModel, const Shader* InShader);
};

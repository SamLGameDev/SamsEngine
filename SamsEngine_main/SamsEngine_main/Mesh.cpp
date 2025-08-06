#include "Mesh.h"
#include "Model.h"
#include "glm-1.0.1/glm/glm.hpp"
#include "glm-1.0.1/glm/gtc/matrix_transform.hpp"
#include "glm-1.0.1/glm/gtc/type_ptr.hpp"
#include "Camera.h"

Mesh::Mesh()
{
}

Mesh::Mesh(Array<Vertex> InVertices, Array<unsigned int> InIndices, Shader InShader)
{
	Vertices = InVertices;
	Indices = InIndices;
	MeshShader = InShader;

	//for (int i = 0; i < Vertices.GetSize(); i++)
	//{
	//	std::cout << "Vert" << Vertices[i].Position.X << "," << Vertices[i].Position.Y << "," << Vertices[i].Position.Z << std::endl;
	//}

	SetUpMesh();
}

Mesh::Mesh(const Mesh& Copy)
{
	Vertices = Copy.Vertices;
	Indices = Copy.Indices;
	MeshShader = Copy.MeshShader;
	VAO = Copy.VAO;
	VBO = Copy.VBO;
	EBO = Copy.EBO;
}

void Mesh::Draw(const Transform* ModelTransform)
{
	MeshShader.Use();

	MeshShader.ApplyTextures();
	glActiveTexture(GL_TEXTURE0);

	SetShaderVariables(ModelTransform);

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, Indices.GetSize(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void Mesh::SetShaderVariables(const Transform* ModelTransform)
{
	glm::mat4 model = ModelTransform->GetModelMatrix();

	glm::mat4 view = Camera::GetActiveCamera()->GetLook();

	glm::mat4 projection = Camera::GetActiveCamera()->GetProjection();

	glm::mat3 normalModel = glm::transpose(glm::inverse(view * model));

	SetTransformationVariables(model, view, projection, normalModel);

	SetLightVariables(view);
}

void Mesh::SetLightVariables(glm::mat4& view)
{
	glm::vec4 lightModel = view * glm::vec4(1.2f, 1.0f, 2.0f, 1);

	MeshShader.SetVec3("light.position", Vector3D(lightModel.x, lightModel.y, lightModel.z));

	MeshShader.SetFloat("material.shininess", 64.0f);

	MeshShader.SetVec3("light.ambient", Vector3D(0.5f, 0.5f, 0.5f));
	MeshShader.SetVec3("light.diffuse", Vector3D(0.2f, 0.2f, 0.2f)); // darken diffuse light a bit
	MeshShader.SetVec3("light.specular", Vector3D(1.0f, 1.0f, 1.0f));
}

void Mesh::SetTransformationVariables(glm::mat4& model, glm::mat4& view, glm::mat4& projection, glm::mat3& normalModel)
{
	MeshShader.SetMatrix4fv("Model", glm::value_ptr(model));

	MeshShader.SetMatrix4fv("View", glm::value_ptr(view));

	MeshShader.SetMatrix4fv("Projection", glm::value_ptr(projection));

	MeshShader.SetMatrix3fv("NormalModel", glm::value_ptr(normalModel));
}

void Mesh::SetUpMesh()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, Vertices.GetSize() * sizeof(Vertex), Vertices.GetFirstRef(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, Indices.GetSize() * sizeof(unsigned int), Indices.GetFirstRef(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));



	glBindVertexArray(0);
}

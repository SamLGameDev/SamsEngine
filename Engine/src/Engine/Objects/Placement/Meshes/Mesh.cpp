#include "Mesh.h"
#include "Model.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Camera.h"
#include "LightManager.h"

Mesh::Mesh()
{
}

Mesh::Mesh(const Array<Vertex>& InVertices, const Array<unsigned int>& InIndices, const Shader& InShader)
{
	Vertices = InVertices;
	Indices = InIndices;
	MeshShader = InShader;

	SetUpMesh();
}

Mesh::Mesh(const Mesh& Copy)
{
	Vertices = Copy.Vertices;
	Indices = Copy.Indices;
	MeshShader = Copy.MeshShader;
	VAO = Copy.VAO;
}

void Mesh::Draw(const Transform* ModelTransform) const
{
	Draw(ModelTransform, &MeshShader);
}

void Mesh::Draw(const Transform* ModelTransform, const Shader* InShader) const
{
	InShader->Use();

	InShader->ApplyTextures();
	glActiveTexture(GL_TEXTURE0);

	SetShaderVariables(ModelTransform, InShader);

	//TODO find a way to separate this from model
	//used for reflection shader
	const glm::vec3 camPos = Camera::GetActiveCamera()->GetPos();
	InShader->SetVec3("cameraPos", Vector3D(camPos.x, camPos.y, camPos.z));

	glBindVertexArray(VAO);
	glDrawElementsInstanced(GL_TRIANGLES, static_cast<GLsizei>(Indices.GetSize()), GL_UNSIGNED_INT, 0, *Instances);
	glBindVertexArray(0);
	glUseProgram(0);
}

void Mesh::SetShaderVariables(const Transform* ModelTransform, const Shader* InShader)
{
	const glm::mat4 model = ModelTransform->GetModelMatrix();

	const glm::mat4 view = Camera::GetActiveCamera()->GetLook();

	const glm::mat3 normalModel = glm::transpose(glm::inverse(view * model));

	SetTransformationVariables(model, normalModel, InShader);

	SetLightVariables(view, InShader);
}

void Mesh::SetLightVariables(const glm::mat4& view, const Shader* InShader)
{
	LightManager::GetLightManager()->ImplementLights(InShader);
}

void Mesh::SetTransformationVariables(const glm::mat4& model, const glm::mat3& normalModel, const Shader* InShader)
{
	InShader->SetMatrix4fv("Model", glm::value_ptr(model));

	InShader->SetMatrix3fv("NormalModel", glm::value_ptr(normalModel));
}

void Mesh::RegenerateMesh()
{
	SetUpMesh();
}

void Mesh::SetUpMesh()
{
	unsigned int VBO, EBO;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, Vertices.GetSize() * sizeof(Vertex), Vertices.GetFirstRef(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, Indices.GetSize() * sizeof(unsigned int), Indices.GetFirstRef(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), static_cast<void*>(0));

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, Normal)));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, TexCoords)));

	glBindVertexArray(0);

	glDeleteBuffers(1, &EBO);
	glDeleteBuffers(1, &VBO);
}

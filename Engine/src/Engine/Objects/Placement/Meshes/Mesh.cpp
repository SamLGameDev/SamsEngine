#include "Mesh.h"
#include "Model.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Camera.h"
#include "DataBuffers.h"
#include "InterfaceRenderer.h"
#include "LightManager.h"
#include "Verticie.h"

Mesh::Mesh()
{
}

Mesh::Mesh(const Array<Vertex>& InVertices, const Array<uint16_t>& InIndices, Shader& InShader)
{

}


Mesh::~Mesh()
{	
}


Mesh::Mesh(const Mesh& copy)
{
	Copy(copy);
}

void Mesh::Copy(const Mesh& Copy)
{
	Vertices = Copy.Vertices;
	Indices = Copy.Indices;
	MeshShader = Copy.MeshShader;
	FVerts = Copy.FVerts;
	FTexCoords = Copy.FTexCoords;
	VAO = Copy.VAO;
	RegenerateMesh();
}

void Mesh::Initialise()
{
	FVerts.Reallocate(Vertices.GetSize());
	FTexCoords.Reallocate(Vertices.GetSize());

	for (size_t i = 0; i < Vertices.GetSize(); i++)
	{
		FVerts[i] = Vertices[i].Position;
		FTexCoords[i] = Vertices[i].TexCoords;
	}

	SetUpMesh();
}

void Mesh::Draw(const Transform* ModelTransform)
{
	Draw(ModelTransform, &MeshShader.value());
}

void Mesh::Draw(const Transform* ModelTransform, ::Shader* InShader) const
{


	InShader->Use();

	DataBuffers::BindBuffer(VAO);

	DataBuffers::DrawVertexData(VAO);


	SetShaderVariables(ModelTransform, InShader);

	::Renderer::Draw(Indices.GetSize());

	//InShader->ApplyTextures();
	//glActiveTexture(GL_TEXTURE0);


	//TODO find a way to separate this from model
	//used for reflection shader
	//const glm::vec3 camPos = Camera::GetActiveCamera()->GetPos();
	//InShader->SetVec3("cameraPos", Vector3D(camPos.x, camPos.y, camPos.z));

	//glBindVertexArray(VAO);
	//glDrawElementsInstanced(GL_TRIANGLES, static_cast<GLsizei>(Indices.GetSize()), GL_UNSIGNED_INT, nullptr, *Instances);
	//glBindVertexArray(0);
	//glUseProgram(0);
}

void Mesh::SetShaderVariables(const Transform* ModelTransform, Shader* InShader)
{

	PerInstanceTransforms ubo;
	ubo.Model = ModelTransform->GetModelMatrix();

	GlobalTransforms g;
	g.View = Camera::GetActiveCamera()->GetLook();

	g.Projection = Camera::GetActiveCamera()->GetProjection();

	InShader->SetUniformBuffer(0, &g, sizeof(GlobalTransforms));

	InShader->SetUniformBuffer(1, &ubo, sizeof(PerInstanceTransforms));
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

//	SetUpMesh();
}

void Mesh::SetUpMesh()
{

	::DataBuffers::GenBuffer(VAO);


	DataBuffers::BindVertexInfo(VAO, 0, 0, sizeof(Vector3D), 0, Vector3);
	DataBuffers::BindVertexInfo(VAO, 1, 0, sizeof(Vector2D), 0, Vector2);

	::DataBuffers::BufferData(VAO, FVerts.GetSize() * sizeof(Vector3D), FVerts.GetFirstRef(), BufferTargets::VERTEX);
	DataBuffers::BufferDataIndex(VAO, Indices.GetSize() * sizeof(uint16_t), Indices.GetFirstRef());
	DataBuffers::BufferData(VAO, FTexCoords.GetSize() * sizeof(Vector2D), FTexCoords.GetFirstRef(), BufferTargets::VERTEX);

}

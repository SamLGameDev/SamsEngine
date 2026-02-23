#include "Mesh.h"
#include "Model.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Camera.h"
#include "DataBuffers.h"
#include "FileSaving.h"
#include "InterfaceRenderer.h"
#include "LightManager.h"
#include "Verticie.h"

Mesh::Mesh()
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
	Draw(ModelTransform, MeshShader);
}

void Mesh::Draw(const Transform* ModelTransform, ::Shader& InShader) const
{
	InShader.Use();

	DataBuffers::BindBuffer(VAO);

	DataBuffers::DrawVertexData(VAO);


	SetShaderVariables(ModelTransform, InShader);

	::Renderer::Draw(Indices.GetSize());
}

void Mesh::SetShaderVariables(const Transform* ModelTransform, Shader InShader)
{

	PerInstanceTransforms ubo;
	ubo.Model = ModelTransform->GetModelMatrix();
	GlobalTransforms g;
	g.View = Camera::GetActiveCamera()->GetLook();

	g.Projection = Camera::GetActiveCamera()->GetProjection();

	InShader.SetUniformBuffer(0, &g, sizeof(GlobalTransforms));

	InShader.SetUniformBuffer(1, &ubo, sizeof(PerInstanceTransforms));
}


void Mesh::RegenerateMesh()
{

//	SetUpMesh();
}

void Mesh::SetUpMesh()
{

	::DataBuffers::GenBuffer(VAO);


	DataBuffers::BindVertexInfo(VAO, 0, 0, sizeof(Vector3D), 0, Vector3);
	::DataBuffers::BufferData(VAO, FVerts.GetSize() * sizeof(Vector3D), FVerts.GetFirstPtr(), BufferTargets::VERTEX);
	DataBuffers::BindVertexInfo(VAO, 1, 0, sizeof(Vector2D), 0, Vector2);

	DataBuffers::BufferDataIndex(VAO, Indices.GetSize() * sizeof(uint16_t), Indices.GetFirstPtr());


	DataBuffers::BufferData(VAO, FTexCoords.GetSize() * sizeof(Vector2D), FTexCoords.GetFirstPtr(), BufferTargets::VERTEX);

}

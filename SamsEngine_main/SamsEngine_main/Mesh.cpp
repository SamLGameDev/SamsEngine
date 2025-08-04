#include "Mesh.h"
#include "Model.h"

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

void Mesh::Draw()
{
	MeshShader.ApplyTextures();
	glActiveTexture(GL_TEXTURE0);

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, Indices.GetSize(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
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

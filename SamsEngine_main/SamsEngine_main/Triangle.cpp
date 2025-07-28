#include "Triangle.h"
#include "Verticie.h"
#include "Vector3D.h"
#include <glad/glad.h>
#include "GLFW/glfw3.h"
#include <iostream>

Triangle::Triangle(Vertex InPos1, Vertex InPos2, Vertex InPos3, Vector3D color, Vector2D TexPos1, Vector2D TexPos2, Vector2D TexPos3)
{
	Verts.Add(InPos1);
	Verts.Add(InPos2);
	Verts.Add(InPos3);

	Vertices = new float[9];

	Vertices[0] = InPos1.Position.X;
	Vertices[1] = InPos1.Position.Y;
	Vertices[2] = InPos1.Position.Z;
	Vertices[3] = InPos2.Position.X;
	Vertices[4] = InPos2.Position.Y;
	Vertices[5] = InPos2.Position.Z;
	Vertices[6] = InPos3.Position.X;
	Vertices[7] = InPos3.Position.Y;
	Vertices[8] = InPos3.Position.Z;

	TextureCords.Add(TexPos1);
	TextureCords.Add(TexPos2);
	TextureCords.Add(TexPos3);

	Color = color;

	shader = Shader("TestShader", "Contents/Shaders/");

	//shader.AddTexture(Texture("Contents/Images/Container.jpg"));
}

Triangle::~Triangle()
{
	delete[] Vertices;
}



void Triangle::Render()
{

}

void Triangle::SetRenderOrder(LinkedList<int> Indices)
{
	IndicesOrder = Indices;
//	IndicesOrder.Sort();
}

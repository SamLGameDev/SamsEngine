#include "Triangle.h"
#include "Verticie.h"
#include "Vector3D.h"
#include <glad/glad.h>
#include "GLFW/glfw3.h"
#include <iostream>


Triangle::Triangle(Verticie InPos1, Verticie InPos2, Verticie InPos3)
{

	Verts.Add(InPos1);
	Verts.Add(InPos2);
	Verts.Add(InPos3);

	Vertices = new float[9];

	Vertices[0] = InPos1.GetPosition().X;
	Vertices[1] = InPos1.GetPosition().Y;
	Vertices[2] = InPos1.GetPosition().Z;
	Vertices[3] = InPos2.GetPosition().X;
	Vertices[4] = InPos2.GetPosition().Y;
	Vertices[5] = InPos2.GetPosition().Z;
	Vertices[6] = InPos3.GetPosition().X;
	Vertices[7] = InPos3.GetPosition().Y;
	Vertices[8] = InPos2.GetPosition().Z;




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

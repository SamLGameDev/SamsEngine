#include "WireShapes.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Array.h"


void DrawWireCube(Vector3D Center, Vector3D Size)
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	Array<float> Vertices;

	Array<unsigned int> Indices;

	Vertices.Add(Center.X + (Size.X / 2));
	Vertices.Add(Center.Y + (Size.Y / 2));
	Vertices.Add(Center.Z + (Size.Z / 2));

	Vertices.Add(Center.X + (Size.X / 2));
	Vertices.Add(Center.Y - (Size.Y / 2));
	Vertices.Add(Center.Z + (Size.Z / 2));

	Vertices.Add(Center.X - (Size.X / 2));
	Vertices.Add(Center.Y - (Size.Y / 2));
	Vertices.Add(Center.Z + (Size.Z / 2));

	Indices.Add(0);
	Indices.Add(1);
	Indices.Add(2);

	Vertices.Add(Center.X - (Size.X / 2));
	Vertices.Add(Center.Y + (Size.Y / 2));
	Vertices.Add(Center.Z + (Size.Z / 2));

	Indices.Add(0);
	Indices.Add(3);
	Indices.Add(2);

	Vertices.Add(Center.X + (Size.X / 2));
	Vertices.Add(Center.Y + (Size.Y / 2));
	Vertices.Add(Center.Z - (Size.Z / 2));

	Vertices.Add(Center.X + (Size.X / 2));
	Vertices.Add(Center.Y - (Size.Y / 2));
	Vertices.Add(Center.Z - (Size.Z / 2));

	Vertices.Add(Center.X - (Size.X / 2));
	Vertices.Add(Center.Y - (Size.Y / 2));
	Vertices.Add(Center.Z - (Size.Z / 2));

	Indices.Add(4);
	Indices.Add(5);
	Indices.Add(6);

	Vertices.Add(Center.X - (Size.X / 2));
	Vertices.Add(Center.Y + (Size.Y / 2));
	Vertices.Add(Center.Z - (Size.Z / 2));

	Indices.Add(4);
	Indices.Add(7);
	Indices.Add(6);

	Indices.Add(0);
	Indices.Add(4);
	Indices.Add(5);

	Indices.Add(0);
	Indices.Add(1);
	Indices.Add(5);

	Indices.Add(3);
	Indices.Add(2);
	Indices.Add(6);

	Indices.Add(3);
	Indices.Add(7);
	Indices.Add(6);

	unsigned int VAO, VBO, EBO;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, Vertices.GetSize() * sizeof(float), Vertices.GetFirstRef(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, Indices.GetSize() * sizeof(unsigned int), Indices.GetFirstRef(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float), (void*)0);

	glBindVertexArray(0);




	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

}

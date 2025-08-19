#include "WireShapes.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Array.h"
#include "Shader.h"
#include "Camera.h"
#include "glm-1.0.1/glm/gtc/type_ptr.hpp"
#include "Renderer.h"

WireObject::WireObject(const Transform* InTransform, const Shader* InShader)
{
	WireTransform = *InTransform;

	WireShader = new Shader(*InShader);

	Renderer::WiresToDraw.Add(this);
}

WireObject::WireObject(const WireObject& Copy)
{
	WireTransform = Copy.WireTransform;
	WireShader = new Shader(*Copy.WireShader);
	Vertices = Copy.Vertices;
	Indices = Copy.Indices;
	VAO = Copy.VAO;
	VBO = Copy.VBO;
	EBO = Copy.EBO;
}

WireObject::~WireObject()
{
	delete WireShader;
}


void WireObject::Initialise()
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
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, Color)));

	glBindVertexArray(0);
}

void WireObject::Draw()
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	WireShader->Use();

	SetShaderVariables(&WireTransform);

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, Indices.GetSize(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glUseProgram(0);

}

void WireObject::SetShaderVariables(const Transform* WireTransform)
{
	glm::mat4 model = WireTransform->GetModelMatrix();

	glm::mat4 view = Camera::GetActiveCamera()->GetLook();

	glm::mat4 projection = Camera::GetActiveCamera()->GetProjection();

	SetTransformationVariables(model, view, projection);

}

void WireObject::SetTransformationVariables(glm::mat4& model, glm::mat4& view, glm::mat4& projection)
{
	WireShader->SetMatrix4fv("Model", glm::value_ptr(model));

	WireShader->SetMatrix4fv("View", glm::value_ptr(view));

	WireShader->SetMatrix4fv("Projection", glm::value_ptr(projection));
}

WireObject* DrawWireCube(Vector3D Center, Vector3D HalfBounds, Vector3D Size, Vector3D Color)
{ 

	Transform wireTransform = Transform(Center, Size, Vector3D(0, 0, 0));

	Shader wireShader = Shader("WireShader", "Contents/Shaders/WireShader/");

	WireObject* object = new WireObject(&wireTransform, &wireShader);
	Vertex Point;

	Point.Position = HalfBounds;

	Point.Color = Color;

	object->Vertices.Add(Point);

	Point.Position = Vector3D(HalfBounds.X, -HalfBounds.Y, HalfBounds.Z);

	object->Vertices.Add(Point);

	Point.Position = Vector3D(-HalfBounds.X, -HalfBounds.Y, HalfBounds.Z);

	object->Vertices.Add(Point);

	object->Indices.Add(0);
	object->Indices.Add(1);
	object->Indices.Add(2);


	Point.Position = Vector3D(-HalfBounds.X, HalfBounds.Y, HalfBounds.Z);

	object->Vertices.Add(Point);

	object->Indices.Add(0);
	object->Indices.Add(3);
	object->Indices.Add(2);

	Point.Position = Vector3D(HalfBounds.X, HalfBounds.Y, -HalfBounds.Z);

	object->Vertices.Add(Point);

	Point.Position = Vector3D(HalfBounds.X, -HalfBounds.Y, -HalfBounds.Z);

	object->Vertices.Add(Point);

	Point.Position = Vector3D(-HalfBounds.X, -HalfBounds.Y, -HalfBounds.Z);

	object->Vertices.Add(Point);

	object->Indices.Add(4);
	object->Indices.Add(5);
	object->Indices.Add(6);

	Point.Position = Vector3D(-HalfBounds.X, HalfBounds.Y, -HalfBounds.Z);

	object->Vertices.Add(Point);

	object->Indices.Add(4);
	object->Indices.Add(7);
	object->Indices.Add(6);

	object->Indices.Add(0);
	object->Indices.Add(4);
	object->Indices.Add(5);

	object->Indices.Add(0);
	object->Indices.Add(1);
	object->Indices.Add(5);

	object->Indices.Add(3);
	object->Indices.Add(2);
	object->Indices.Add(6);

	object->Indices.Add(3);
	object->Indices.Add(7);
	object->Indices.Add(6);



	Face face;
	face.Verticies.Add(object->Vertices[object->Indices[0]]);
	face.Verticies.Add(object->Vertices[object->Indices[1]]);
	face.Verticies.Add(object->Vertices[object->Indices[2]]);
	face.Verticies.Add(object->Vertices[object->Indices[3]]);
	object->Faces.Add(face);

	face = Face();
	face.Verticies.Add(object->Vertices[object->Indices[0]]);
	face.Verticies.Add(object->Vertices[object->Indices[1]]);
	face.Verticies.Add(object->Vertices[object->Indices[5]]);
	face.Verticies.Add(object->Vertices[object->Indices[4]]);
	object->Faces.Add(face);

	face = Face();
	face.Verticies.Add(object->Vertices[object->Indices[4]]);
	face.Verticies.Add(object->Vertices[object->Indices[5]]);
	face.Verticies.Add(object->Vertices[object->Indices[6]]);
	face.Verticies.Add(object->Vertices[object->Indices[7]]);
	object->Faces.Add(face);

	face = Face();
	face.Verticies.Add(object->Vertices[object->Indices[2]]);
	face.Verticies.Add(object->Vertices[object->Indices[3]]);
	face.Verticies.Add(object->Vertices[object->Indices[7]]);
	face.Verticies.Add(object->Vertices[object->Indices[6]]);
	object->Faces.Add(face);

	//face = Face();
	//face.Verticies.Add(object->Vertices[object->Indices[0]]);
	//face.Verticies.Add(object->Vertices[object->Indices[3]]);
	//face.Verticies.Add(object->Vertices[object->Indices[7]]);
	//face.Verticies.Add(object->Vertices[object->Indices[3]]);
	//object->Faces.Add(face);

	//face = Face();
	//face.Verticies.Add(object->Vertices[object->Indices[1]]);
	//face.Verticies.Add(object->Vertices[object->Indices[2]]);
	//face.Verticies.Add(object->Vertices[object->Indices[6]]);
	//face.Verticies.Add(object->Vertices[object->Indices[5]]);
	//object->Faces.Add(face);


	object->Initialise();

	return object;

}

WireObject* DrawWirePlane(Vector3D Center, Vector3D Normal, Vector3D Size, Vector3D Color)
{
	Transform wireTransform = Transform(Center, Size, Vector3D(0, 0, 0));

	Shader wireShader = Shader("WireShader", "Contents/Shaders/WireShader/");

	WireObject* object = new WireObject(&wireTransform, &wireShader);

	Vector3D arbitraryUp = Vector3D::Up;
	if (fabs(Vector3D::Dot(Normal, arbitraryUp)) > 0.99f) {
		arbitraryUp = Vector3D(1, 0, 0);  // pick another axis if too close
	}

	Vector3D Right = Vector3D::Cross(Normal, arbitraryUp);

	Vector3D Up = Vector3D::Cross(Right, Normal);


	Vertex Point;

	Point.Color = Color;

	Point.Position = Up + Right;

	object->Vertices.Add(Point);

	Point.Position = Up - Right;

	object->Vertices.Add(Point);

	Point.Position =  -Up + Right;

	object->Vertices.Add(Point);

	Point.Position = -Up - Right;

	object->Vertices.Add(Point);

	object->Indices.Add(0);
	object->Indices.Add(1);
	object->Indices.Add(3);

	object->Indices.Add(0);
	object->Indices.Add(3);
	object->Indices.Add(2);
	

	for (unsigned int Ind = 0; Ind + 2 < object->Indices.GetSize(); Ind += 3)
	{
		Face face;
		face.Verticies.Add(object->Vertices[object->Indices[Ind]]);
		face.Verticies.Add(object->Vertices[object->Indices[Ind + 1]]);
		face.Verticies.Add(object->Vertices[object->Indices[Ind + 2]]);
		object->Faces.Add(face);
	}

	object->Initialise();

	return object;

	return nullptr;
}

WireObject* DrawWireLine(Vector3D Start, Vector3D End, Vector3D Color)
{
	Transform wireTransform = Transform(Start, Vector3D(1,1,1), Vector3D(0, 0, 0));

	Shader wireShader = Shader("WireShader", "Contents/Shaders/WireShader/");

	WireObject* object = new WireObject(&wireTransform, &wireShader);


	Vertex Point;

	Point.Color = Color;

	Point.Position = Vector3D::Zero;

	object->Vertices.Add(Point);

	Point.Position = End - Start;

	object->Vertices.Add(Point);

	object->Indices.Add(0);
	object->Indices.Add(1);
	object->Indices.Add(0);

	for (unsigned int Ind = 0; Ind + 2 < object->Indices.GetSize(); Ind += 3)
	{
		Face face;
		face.Verticies.Add(object->Vertices[object->Indices[Ind]]);
		face.Verticies.Add(object->Vertices[object->Indices[Ind + 1]]);
		face.Verticies.Add(object->Vertices[object->Indices[Ind + 2]]);
		object->Faces.Add(face);
	}

	object->Initialise();

	return object;
}

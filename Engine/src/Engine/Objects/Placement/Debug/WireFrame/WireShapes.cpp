// DO NOT MARK except lines 139 - 190 as marked by the DO MARK comments.
//This is because it has been submitted for my dissertation. Link to Original: https://github.falmouth.ac.uk/GA-Undergrad-Student-Work-25-26/Dissertation-SL295211.git



#include "WireShapes.h"
#include "Array.h"
#include "Shader.h"
#include "Camera.h"
#include <glm/gtc/type_ptr.hpp>
#include "InterfaceRenderer.h"
#include "Verticie.h"
#include "Transform.h"

WireObject::WireObject(const Transform* InTransform, const Shader* InShader)
{
	WireTransform = *InTransform;

	WireShader = std::make_shared<Shader>(*InShader);

	//::Renderer::AddWireShape(this);
}

void WireObject::Copy(const WireObject& copy)
{
	WireTransform = copy.WireTransform;
	WireShader = copy.WireShader;
	Vertices = copy.Vertices;
	Indices = copy.Indices;
	Initialise();
}

WireObject::WireObject(const WireObject& copy)
{
	Copy(copy);
}

WireObject::~WireObject()
{

	//glDeleteBuffers(1, &EBO);
	//glDeleteBuffers(1, &VBO);
	//glDeleteVertexArrays(1, &VAO);
}

void WireObject::Draw() const
{
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	//WireShader->Use();

	//SetShaderVariables();

	//glBindVertexArray(VAO);
	//glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(Indices.GetSize()), GL_UNSIGNED_INT, 0);
	//glBindVertexArray(0);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	//glUseProgram(0);
}

void WireObject::Initialise()
{

	//glGenVertexArrays(1, &VAO);
	//glGenBuffers(1, &VBO);
	//glGenBuffers(1, &EBO);

	//glBindVertexArray(VAO);
	//glBindBuffer(GL_ARRAY_BUFFER, VBO);

	//glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * Vertices.GetSize(), Vertices.GetFirstPtr(), GL_STATIC_DRAW);

	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * Indices.GetSize(), Indices.GetFirstPtr(), GL_STATIC_DRAW);

	//glEnableVertexAttribArray(0);
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), static_cast<void*>(0));

	//glEnableVertexAttribArray(1);
	//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>((offsetof(Vertex, Color))));

	//glBindVertexArray(0);
}

void WireObject::SetShaderVariables() const
{
	const glm::mat4 model = WireTransform.GetModelMatrix();

	SetTransformationVariables(model);
}

void WireObject::SetTransformationVariables(const glm::mat4& model) const
{
	//WireShader->SetMatrix4fv("Model", glm::value_ptr(model));
}

std::unique_ptr<WireObject> DrawWireCube(const Vector3D& Center, const Vector3D& HalfBounds, const Vector3D& Size,
                                         const Vector3D& Color)
{
	auto wireTransform = Transform(Center, Size, Vector3D(0, 0, 0));

	auto wireShader = Shader("triangle", "/Shaders/");

	auto object = std::make_unique<WireObject>(&wireTransform, &wireShader);

	object->Vertices =
	{
		Vertex(HalfBounds, Color),
		Vertex(Vector3D(HalfBounds.X, -HalfBounds.Y, HalfBounds.Z), Color),
		Vertex(Vector3D(-HalfBounds.X, -HalfBounds.Y, HalfBounds.Z), Color),
		Vertex(Vector3D(-HalfBounds.X, HalfBounds.Y, HalfBounds.Z), Color),
		Vertex(Vector3D(HalfBounds.X, HalfBounds.Y, -HalfBounds.Z), Color),
		Vertex(Vector3D(HalfBounds.X, -HalfBounds.Y, -HalfBounds.Z), Color),
		Vertex(Vector3D(-HalfBounds.X, -HalfBounds.Y, -HalfBounds.Z), Color),
		Vertex(Vector3D(-HalfBounds.X, HalfBounds.Y, -HalfBounds.Z), Color)
	};

	object->Indices =
	{
		//Front
		0, 2, 1,
		0, 3, 2,

		//Back
		4, 5 ,6,
		4, 6, 7,

		//Right
		0, 5, 4,
		0, 1, 5,

		//Left
		3, 6, 2,
		3, 7, 6,
	};

	//DO MARK

	//Top right, bottom right, bottom left, top left
	Face face;
	face.Vertices.Add(object->Vertices[0].Position);
	face.Vertices.Add(object->Vertices[1].Position);
	face.Vertices.Add(object->Vertices[2].Position);
	face.Vertices.Add(object->Vertices[3].Position);
	object->Faces.Add(face);

	//back top right, back bottom right, front bottom right, front top right
	face = Face();
	face.Vertices.Add(object->Vertices[4].Position);
	face.Vertices.Add(object->Vertices[5].Position);
	face.Vertices.Add(object->Vertices[1].Position);
	face.Vertices.Add(object->Vertices[0].Position);
	object->Faces.Add(face);

	//back top left, back bottom left, back bottom right, back top right
	face = Face();
	face.Vertices.Add(object->Vertices[7].Position);
	face.Vertices.Add(object->Vertices[6].Position);
	face.Vertices.Add(object->Vertices[5].Position);
	face.Vertices.Add(object->Vertices[4].Position);
	object->Faces.Add(face);


	//front top left, front bottom left, back bottom left, back top left
	face = Face();
	face.Vertices.Add(object->Vertices[3].Position);
	face.Vertices.Add(object->Vertices[2].Position);
	face.Vertices.Add(object->Vertices[6].Position);
	face.Vertices.Add(object->Vertices[7].Position);
	object->Faces.Add(face);

	//back top right, front top right, front top left, back top left
	face = Face();
	face.Vertices.Add(object->Vertices[4].Position);
	face.Vertices.Add(object->Vertices[0].Position);
	face.Vertices.Add(object->Vertices[3].Position);
	face.Vertices.Add(object->Vertices[7].Position);
	object->Faces.Add(face);

	//back bottom right, front bottom right, front bottom left, back bottom left
	face = Face();
	face.Vertices.Add(object->Vertices[5].Position);
	face.Vertices.Add(object->Vertices[1].Position);
	face.Vertices.Add(object->Vertices[2].Position);
	face.Vertices.Add(object->Vertices[6].Position);
	object->Faces.Add(face);

	//END OF DO MARK

	object->Initialise();

	return object;
}

std::unique_ptr<WireObject> DrawWirePlane(const Vector3D& Center, const Vector3D& Normal, const Vector3D& Size, const Vector3D& Color)
{
	auto wireTransform = Transform(Center, Size, Vector3D(0, 0, 0));

	auto wireShader = Shader("WireShader", "Shaders/WireShader/");

	auto object = std::make_unique<WireObject>(&wireTransform, &wireShader);

	Vector3D arbitraryUp = Vector3D::Up;
	if (fabs(Vector3D::Dot(Normal, arbitraryUp)) > 0.99f) {
		arbitraryUp = Vector3D(1, 0, 0);  //pick another axis if too close
	}

	const Vector3D Right = Vector3D::Cross(Normal, arbitraryUp);

	const Vector3D Up = Vector3D::Cross(Right, Normal);

	object->Vertices =
	{
		Vertex(Up + Right, Color),
		Vertex(Up - Right, Color),
		Vertex(-Up + Right, Color),
		Vertex( -Up - Right, Color),
	};

	object->Indices =
	{
		0, 1, 3,
		0, 3, 2
	};

	//Aren't used currently, when i get back to fracturing maybe

	// for (unsigned int Ind = 0; Ind + 2 < object->Indices.GetSize(); Ind += 3)
	// {
	// 	Face face;
	// 	face.Verticies.Add(object->Vertices[object->Indices[Ind]]);
	// 	face.Verticies.Add(object->Vertices[object->Indices[Ind + 1]]);
	// 	face.Verticies.Add(object->Vertices[object->Indices[Ind + 2]]);
	// 	object->Faces.Add(face);
	// }

	object->Initialise();

	return object;
}

std::unique_ptr<WireObject> DrawWireLine(const Vector3D& Start, const Vector3D& End, const Vector3D& Color)
{
	auto wireTransform = Transform(Start, Vector3D(1, 1, 1), Vector3D(0, 0, 0));

	//TODO update this for GL_Line_SEGMENTS instead, needs a specific geom, and update to wire object

	auto wireShader = Shader("WireShader", "Shaders/WireShader/");

	auto object = std::make_unique<WireObject>(&wireTransform, &wireShader);

	object->Vertices =
	{
		Vertex(Vector3D::Zero, Color),
		Vertex(End -Start, Color),
	};

	object->Indices =
	{
		0, 1, 0
	};

	//Not used currently, maybe when im back on fracturing

	// for (unsigned int Ind = 0; Ind + 2 < object->Indices.GetSize(); Ind += 3)
	// {
	// 	Face face;
	// 	face.Verticies.Add(object->Vertices[object->Indices[Ind]]);
	// 	face.Verticies.Add(object->Vertices[object->Indices[Ind + 1]]);
	// 	face.Verticies.Add(object->Vertices[object->Indices[Ind + 2]]);
	// 	object->Faces.Add(face);
	// }

	object->Initialise();

	return object;
}

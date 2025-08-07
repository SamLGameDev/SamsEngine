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

	glBufferData(GL_ARRAY_BUFFER, Vertices.GetSize() * sizeof(float), Vertices.GetFirstRef(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, Indices.GetSize() * sizeof(unsigned int), Indices.GetFirstRef(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));

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

void DrawWireCube(Vector3D Center, Vector3D HalfBounds, Vector3D Color)
{ 

	Transform wireTransform = Transform(Center, Vector3D(1, 1, 1), Vector3D(0, 0, 0));

	Shader wireShader = Shader("WireShader", "Contents/Shaders/WireShader/");

	WireObject* object = new WireObject(&wireTransform, &wireShader);

	object->Vertices.Add(Center.X + (HalfBounds.X));
	object->Vertices.Add(Center.Y + (HalfBounds.Y));
	object->Vertices.Add(Center.Z + (HalfBounds.Z));

	object->Vertices.Add(Color.X);
	object->Vertices.Add(Color.Y);
	object->Vertices.Add(Color.Z);



	object->Vertices.Add(Center.X + (HalfBounds.X));
	object->Vertices.Add(Center.Y - (HalfBounds.Y));
	object->Vertices.Add(Center.Z + (HalfBounds.Z));


	object->Vertices.Add(Color.X);
	object->Vertices.Add(Color.Y);
	object->Vertices.Add(Color.Z);


	object->Vertices.Add(Center.X - (HalfBounds.X));
	object->Vertices.Add(Center.Y - (HalfBounds.Y));
	object->Vertices.Add(Center.Z + (HalfBounds.Z));


	object->Vertices.Add(Color.X);
	object->Vertices.Add(Color.Y);
	object->Vertices.Add(Color.Z);


	object->Indices.Add(0);
	object->Indices.Add(1);
	object->Indices.Add(2);

	object->Vertices.Add(Center.X - (HalfBounds.X));
	object->Vertices.Add(Center.Y + (HalfBounds.Y));
	object->Vertices.Add(Center.Z + (HalfBounds.Z));


	object->Vertices.Add(Color.X);
	object->Vertices.Add(Color.Y);
	object->Vertices.Add(Color.Z);


	object->Indices.Add(0);
	object->Indices.Add(3);
	object->Indices.Add(2);

	object->Vertices.Add(Center.X + (HalfBounds.X));
	object->Vertices.Add(Center.Y + (HalfBounds.Y));
	object->Vertices.Add(Center.Z - (HalfBounds.Z));


	object->Vertices.Add(Color.X);
	object->Vertices.Add(Color.Y);
	object->Vertices.Add(Color.Z);


	object->Vertices.Add(Center.X + (HalfBounds.X));
	object->Vertices.Add(Center.Y - (HalfBounds.Y));
	object->Vertices.Add(Center.Z - (HalfBounds.Z));


	object->Vertices.Add(Color.X);
	object->Vertices.Add(Color.Y);
	object->Vertices.Add(Color.Z);


	object->Vertices.Add(Center.X - (HalfBounds.X));
	object->Vertices.Add(Center.Y - (HalfBounds.Y));
	object->Vertices.Add(Center.Z - (HalfBounds.Z));


	object->Vertices.Add(Color.X);
	object->Vertices.Add(Color.Y);
	object->Vertices.Add(Color.Z);


	object->Indices.Add(4);
	object->Indices.Add(5);
	object->Indices.Add(6);

	object->Vertices.Add(Center.X - (HalfBounds.X));
	object->Vertices.Add(Center.Y + (HalfBounds.Y));
	object->Vertices.Add(Center.Z - (HalfBounds.Z));


	object->Vertices.Add(Color.X);
	object->Vertices.Add(Color.Y);
	object->Vertices.Add(Color.Z);


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

	object->Initialise();

}

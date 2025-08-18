#pragma once

#include "Vector3D.h"
#include "Transform.h"
#include "Shader.h"
#include "Verticie.h"

class WireObject
{
public:

	WireObject() {};

	WireObject(const Transform* Transform, const Shader* InShader);

	WireObject(const WireObject& Copy);

	~WireObject();

	void Draw();

	Array<Vertex*> Vertices;

	Array<unsigned int> Indices;

	Array<Face> Faces;

	void Initialise();

	Shader* GetShader()
	{
		return WireShader;
	}


	Transform WireTransform;

private:


		unsigned int VAO, VBO, EBO;

		Shader* WireShader;


		void SetShaderVariables(const Transform* WireTransform);


		void SetTransformationVariables(glm::mat4& model, glm::mat4& view, glm::mat4& projection);
};

WireObject* DrawWireCube(Vector3D Center, Vector3D HalfBounds, Vector3D Size, Vector3D Color);

WireObject* DrawWirePlane(Vector3D Center, Vector3D Normal, Vector3D Size, Vector3D Color);

WireObject* DrawWireLine(Vector3D Start, Vector3D End, Vector3D Color);

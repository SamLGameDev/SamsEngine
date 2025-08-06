#pragma once

#include "Vector3D.h"
#include "Transform.h"
#include "Shader.h"

class WireObject
{
public:

	WireObject() {};

	WireObject(const Transform* Transform, const Shader* InShader);

	WireObject(const WireObject& Copy);

	void Draw();

	Array<float> Vertices;

	Array<unsigned int> Indices;

	void Initialise();

	Shader* GetShader()
	{
		return &WireShader;
	}

private:

		Transform WireTransform;

		unsigned int VAO, VBO, EBO;

		Shader WireShader;


		void SetShaderVariables(const Transform* WireTransform);


		void SetTransformationVariables(glm::mat4& model, glm::mat4& view, glm::mat4& projection);
};


WireObject DrawWireCube(Vector3D Center, Vector3D Size, Vector3D Color);

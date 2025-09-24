#pragma once

#include "Vector3D.h"
#include "Transform.h"
#include "Shader.h"
#include "Verticie.h"
#include <memory>


class WireObject
{
public:

	WireObject() {};

	WireObject(const Transform* InTransform, const Shader* InShader);

	WireObject(const WireObject& Copy);

	~WireObject();

	void Draw() const;

	Array<Vertex> Vertices;

	Array<unsigned int> Indices;

	Array<Face> Faces;


	/**
	 * Create the VAO, VBO, and EBO of the object,and pass it to the GPU
	 */
	void Initialise();

	std::weak_ptr<Shader> GetShader()
	{
		return WireShader;
	}

	Transform WireTransform;

private:

	unsigned int VAO;

	std::shared_ptr<Shader> WireShader;

	void SetShaderVariables() const;

	void SetTransformationVariables(const glm::mat4& model) const;
};

std::unique_ptr<WireObject> DrawWireCube(const Vector3D& Center, const Vector3D& HalfBounds, const Vector3D& Size,
                                         const Vector3D& Color);

std::unique_ptr<WireObject> DrawWirePlane(const Vector3D& Center, const Vector3D& Normal, const Vector3D& Size, const Vector3D& Color);

std::unique_ptr<WireObject> DrawWireLine(const Vector3D& Start, const Vector3D& End, const Vector3D& Color);

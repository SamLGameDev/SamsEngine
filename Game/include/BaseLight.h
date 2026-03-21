


#pragma once

#include "Vector3D.h"
#include "Shader.h"

class BaseLight
{
public:

	BaseLight() = default;

	virtual ~BaseLight() = default;

	virtual void Implement(const Shader* InShader) = 0;

protected:

	Vector3D ambient, diffuse, specular;

	float Shininess;

	unsigned int ID;
};

// DO NOT MARK.
//This is because it has been submitted for my dissertation. Link to Original: https://github.falmouth.ac.uk/GA-Undergrad-Student-Work-25-26/Dissertation-SL295211.git


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

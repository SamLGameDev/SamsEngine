// DO NOT MARK.
//This is because it has been submitted for my dissertation. Link to Original: https://github.falmouth.ac.uk/GA-Undergrad-Student-Work-25-26/Dissertation-SL295211.git


#pragma once
#include "BaseLight.h"
class DirectionalLight final : public BaseLight
{
public:

	DirectionalLight(const Vector3D& Ambient, const Vector3D& Diffuse, const Vector3D& Specular, float InShininess);

	void Implement(const Shader* InShader)override;
};

#pragma once
#include "BaseLight.h"
class PointLight final : public BaseLight
{
public:

	PointLight(const Vector3D& Pos, const Vector3D& Ambient, const Vector3D& Diffuse, const Vector3D& Specular, float InShininess, float Constant,
	           float Linear, float Quadratic);

	void Implement(const Shader* InShader)override;

protected:

	float constant, linear, quadratic;

	Vector3D Position;
};

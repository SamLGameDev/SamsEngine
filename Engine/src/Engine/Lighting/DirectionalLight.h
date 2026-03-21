


#pragma once
#include "BaseLight.h"
class DirectionalLight final : public BaseLight
{
public:

	DirectionalLight(const Vector3D& Ambient, const Vector3D& Diffuse, const Vector3D& Specular, float InShininess);

	void Implement(const Shader* InShader)override;
};

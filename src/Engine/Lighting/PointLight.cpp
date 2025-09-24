#include "PointLight.h"
#include "Camera.h"
#include "LightManager.h"

PointLight::PointLight(const Vector3D& Pos, const Vector3D& Ambient, const Vector3D& Diffuse, const Vector3D& Specular,
                       const float InShininess, const float Constant, const float Linear, const float Quadratic)
{
	ambient = Ambient;
	diffuse = Diffuse;
	specular = Specular;

	Shininess = InShininess;

	constant = Constant;
	linear = Linear;
	quadratic = Quadratic;

	Position = Pos;

	ID = LightManager::GetLightManager()->GetPointLightID();

	LightManager::GetLightManager()->AddToLights(this);
}

void PointLight::Implement(const Shader* InShader)
{
	const glm::vec4 lightModel = Camera::GetActiveCamera()->GetLook() * glm::vec4(Position.X, Position.Y, Position.Z, 1);

	const std::string id = "pointLights[" + std::to_string(ID) + "]";

	InShader->SetVec3(id + ".position", Vector3D(lightModel.x, lightModel.y, lightModel.z));

	InShader->SetFloat(id + ".constant", constant);

	InShader->SetFloat(id + ".linear", linear);

	InShader->SetFloat(id + ".quadratic", quadratic);
	InShader->SetFloat("material.shininess", Shininess);

	InShader->SetVec3(id + ".ambient", ambient);
	InShader->SetVec3(id + ".diffuse", diffuse);
	InShader->SetVec3(id + ".specular", specular);
}
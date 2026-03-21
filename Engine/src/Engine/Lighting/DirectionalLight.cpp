

#include "DirectionalLight.h"
#include "Camera.h"
#include "LightManager.h"

DirectionalLight::DirectionalLight(const Vector3D& Ambient, const Vector3D& Diffuse, const Vector3D& Specular, const float InShininess)
{
	ambient = Ambient;
	diffuse = Diffuse;
	specular = Specular;

	Shininess = InShininess;

	LightManager::GetLightManager()->AddToLights(this);
}

void DirectionalLight::Implement(const Shader* InShader)
{
	const auto front = Vector3D(Camera::GetActiveCamera()->GetFront().x, Camera::GetActiveCamera()->GetFront().y, Camera::GetActiveCamera()->GetFront().z);

	//InShader->SetVec3("directional.direction", front);

//	InShader->SetFloat("material.shininess", Shininess);

	//->SetVec3("directional.ambient", ambient);
	//InShader->SetVec3("directional.diffuse", diffuse);
	//InShader->SetVec3("directional.specular", specular);
}

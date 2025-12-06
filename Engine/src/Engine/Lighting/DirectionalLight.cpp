
// DO NOT MARK.
//This is because it has been submitted for my dissertation. Link to Original: https://github.falmouth.ac.uk/GA-Undergrad-Student-Work-25-26/Dissertation-SL295211.git

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

	InShader->SetVec3("directional.direction", front);

	InShader->SetFloat("material.shininess", Shininess);

	InShader->SetVec3("directional.ambient", ambient);
	InShader->SetVec3("directional.diffuse", diffuse);
	InShader->SetVec3("directional.specular", specular);
}

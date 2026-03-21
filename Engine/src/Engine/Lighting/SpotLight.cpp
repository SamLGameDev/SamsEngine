


#include "SpotLight.h"
#include "Camera.h"
#include "LightManager.h"

SpotLight::SpotLight(const Vector3D& Ambient, const Vector3D& Diffuse, const Vector3D& Specular,
	const float InShininess, const float InnerCutOff, const float OuterCutOff)
{
	ambient = Ambient;
	diffuse = Diffuse;
	specular = Specular;

	Shininess = InShininess;

	innerCutOff = InnerCutOff;
	outerCutOff = OuterCutOff;

	LightManager::GetLightManager()->AddToLights(this);
}

void SpotLight::Implement(const Shader* InShader)
{
	const glm::vec4 lightModel = Camera::GetActiveCamera()->transform.GetModelMatrix() * glm::vec4(0, 0, 0, 1);

	//InShader->SetVec3("spotLight.position", Vector3D(lightModel.x, lightModel.y, lightModel.z));

	const glm::vec3 front = Camera::GetActiveCamera()->GetFront();

//	InShader->SetVec3("spotLight.direction", Vector3D(front.x, front.y, front.z));

	//InShader->SetFloat("material.shininess", Shininess);

	//InShader->SetVec3("spotLight.ambient", ambient);
	//->SetVec3("spotLight.diffuse", diffuse);
	//InShader->SetVec3("spotLight.specular", specular);

//	InShader->SetFloat("spotLight.innerCutOff", innerCutOff);
//	InShader->SetFloat("spotLight.outerCutOff", outerCutOff);
}

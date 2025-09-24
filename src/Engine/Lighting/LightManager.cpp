#include "LightManager.h"

LightManager* LightManager::Manager;

LightManager::LightManager()
{
	Manager = this;
}

LightManager::~LightManager()
{
}

void LightManager::ImplementLights(const Shader* InShader) const
{
	for (BaseLight* light : Lights)
	{
		light->Implement(InShader);
	}
}

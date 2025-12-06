// DO NOT MARK.
//This is because it has been submitted for my dissertation. Link to Original: https://github.falmouth.ac.uk/GA-Undergrad-Student-Work-25-26/Dissertation-SL295211.git

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

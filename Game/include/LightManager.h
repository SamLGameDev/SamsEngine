#pragma once

#include "BaseLight.h"
#include "Array.h"

class LightManager
{
public:

	LightManager();

	~LightManager();

	[[nodiscard]] unsigned int GetPointLightID()
	{
		const unsigned int id = PointLightNum;
		PointLightNum++;
		return id;
	}

	void ImplementLights(const Shader* InShader) const;

	void AddToLights(BaseLight* InLight)
	{
		Lights.Add(InLight);
	}

	[[nodiscard]] static LightManager* GetLightManager()
	{
		return Manager;
	}

private:

	unsigned int PointLightNum = 0;

	Array<BaseLight*> Lights;

	static LightManager* Manager;
};

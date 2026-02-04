#include "OpenGLInstance.h"

#include <exception>
#include <iostream>
#include "WorldObject.h"
#include "ObjectFactory.h"

namespace OpenGL
{

	std::optional<SInstance*> SInstance::Instance;

	
	ErrorCodes SInstance::ShutDown()
	{
		return SUCCEEDED;
	}

	SInstance* SInstance::GetInstance()
	{
		if (Instance.has_value())
		{
			return Instance.value();
		}

		Instance = new SInstance();

		return Instance.value();
	}
}

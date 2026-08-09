#pragma once
#include "ErrorCodes.h"

class BaseRuntimeEngine
{
public:

	virtual ~BaseRuntimeEngine() = default;

	virtual ErrorCodes Init() = 0;

	//virtual void 
};

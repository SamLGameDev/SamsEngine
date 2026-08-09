#pragma once
#include <thread>

#include "Application.h"

class URenderThread
{
public:

	URenderThread(const EGraphicsAPI API);

	bool ShouldClose();

protected:

	virtual void Run(const EGraphicsAPI API);

	std::thread Thread;

};

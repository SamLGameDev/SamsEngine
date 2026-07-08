#pragma once
#include <thread>

class UWorkerThread
{
public:

	UWorkerThread();


protected:

	virtual void Run();

	std::thread Thread;

};

#pragma once
#include <thread>

class UGameThread
{
public:

	UGameThread();


protected:

	virtual void Run();

	std::thread Thread;

};

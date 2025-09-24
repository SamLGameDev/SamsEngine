#include "SubsystemInitialiser.h"

#include <exception>
#include <__msvc_filebuf.hpp>
#include <iostream>

ErrorCodes SubsystemInitialiser::Init()
{
	try
	{
		Window = FirstWindow();
	}
	catch (const std::exception& error)
	{
		std::cout << error.what() << "\n";
		return ERROR;
	}

	return SUCCEEDED;
}


#pragma once

#include "Path.h"

class CorePaths
{
public:

	CorePaths();

	/// <summary>
	/// The path to the contents directory, Engine needs to be initialized before this is valid
	/// </summary>
	static FPath Contents;

};

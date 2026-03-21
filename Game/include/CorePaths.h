// DO NOT MARK 
//This is because it has been submitted for my COMP305. Link to Original: https://github.falmouth.ac.uk/GA-Undergrad-Student-Work-25-26/Comp305-Engine-SL295211.git
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

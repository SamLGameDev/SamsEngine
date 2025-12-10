#pragma once
#include <inttypes.h>

//TODO Should probably replace this with std error stuff? not sure could be beneficial to having my own if i build it out more

/// <summary>
/// Use to indiacte success, warning or error states of functions
/// </summary>
enum ErrorCodes : uint8_t
{
	SUCCEEDED,
	WARNING,
	ERROR

};

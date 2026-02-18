
#include "HardwareDetails.h"

#include <iostream>
#include <windows.h>
#include "stdio.h"

#if defined(_WIN32) || defined(_WIN64) || defined(_CYGWIN)
#define PLATFORM_NAME "Windows"
#elif defined(unix) || defined(__unix) || defined(__unix__)
#define PLATFORM_NAME "Unix"
#elif defined(_APPLE__) || defined(__MACH__)
#define PLATFORM_NAME "Mac"
#elif defined(__linux__)
#define PLATFORM_NAME "Linux"
#else
#define PLATFORM_NAME NULL
#endif

std::function<std::string()> UHardwareDetails::GetGPU;

std::string UHardwareDetails::API;



std::string UHardwareDetails::GetOS()
{
	return PLATFORM_NAME == NULL ? "" : PLATFORM_NAME;
}

std::string UHardwareDetails::GetRAM_GB()
{
	MEMORYSTATUSEX status;
	status.dwLength = sizeof(status);
	GlobalMemoryStatusEx(&status);
	return std::to_string(status.ullTotalPhys / 1073741824);

}

std::string UHardwareDetails::GetCPU()
{
	int CPUInfo[4] = { -1 };
	char CPUBrandString[0x40];
	__cpuid(CPUInfo, 0x80000000);
	unsigned int nExIds = CPUInfo[0];

	memset(CPUBrandString, 0, sizeof(CPUBrandString));

	// Get the information associated with each extended ID.
	for (int i = 0x80000000; i <= nExIds; ++i)
	{
		__cpuid(CPUInfo, i);
		// Interpret CPU brand string.
		if (i == 0x80000002)
			memcpy(CPUBrandString, CPUInfo, sizeof(CPUInfo));
		else if (i == 0x80000003)
			memcpy(CPUBrandString + 16, CPUInfo, sizeof(CPUInfo));
		else if (i == 0x80000004)
			memcpy(CPUBrandString + 32, CPUInfo, sizeof(CPUInfo));
	}
	return CPUBrandString;
}

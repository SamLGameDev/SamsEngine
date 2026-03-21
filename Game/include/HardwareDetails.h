
#pragma once
#include <string>
#include <functional>

class UHardwareDetails
{
public:

	static std::string GetOS();

	static std::string GetRAM_GB();

	static std::string GetCPU();

	static std::function<std::string()> GetGPU;

	static std::string API;

};

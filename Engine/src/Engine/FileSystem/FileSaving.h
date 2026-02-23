
#pragma once

#include <string>

#include "Vector3D.h"

struct UFileWriter
{
public:

	static void SaveArray(const std::string_view& Name, const Array<Vector3D>& Data);
	static void Load(const std::string_view& Name, Array<Vector3D>& Data);
	static void Load(const std::string_view& Name, Array<Vector3D>& Data, const Vector2D& Range);
	static std::string ReadFileContents(const std::string_view& Location) ;
};

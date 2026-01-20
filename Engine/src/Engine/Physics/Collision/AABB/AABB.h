// DO NOT MARK.
//This is because it has been submitted for my dissertation. Link to Original: https://github.falmouth.ac.uk/GA-Undergrad-Student-Work-25-26/Dissertation-SL295211.git


#pragma once

#include "Array.h"
#include "Vector3D.h"

struct FBox
{
	Vector3D min;
	Vector3D max;


	FBox();
	FBox(const Array<Vector3D>& Points)
	{
		for (const auto& point : Points)
		{
			min.X = std::min(min.X, point.X);
			min.Y = std::min(min.Y, point.Y);
			min.Z = std::min(min.Z, point.Z);
			max.X = std::max(max.X, point.X);
			max.Y = std::max(max.Y, point.Y);
			max.Z = std::max(max.Z, point.Z);
		}
	}

};

class AABB
{
public:
	static bool IsPointInsideBox(const FBox& Box, const Vector3D& Point);
}
;

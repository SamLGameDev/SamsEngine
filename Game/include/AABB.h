

#pragma once

#include "Array.h"
#include "Vector3D.h"
#include "Verticie.h"

struct FBox
{
	Vector3D min = Vector3D::NumericMax;
	Vector3D max = Vector3D::NumericMin;


	FBox() = default;
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

	Array<Face> GetFaces() const;

};

class AABB
{
public:
	static bool IsPointInsideBox(const FBox& Box, const Vector3D& Point);

	static bool IsBoxIntersectingBox(const FBox& Box1, const FBox& Box2);
}
;

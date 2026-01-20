#include "AABB.h"

bool AABB::IsPointInsideBox(const FBox& Box, const Vector3D& Point)
{
	return Point >= Box.min && Point <= Box.max;
}


#pragma once

#include "ErrorCodes.h"
#include "SubsystemInitialiser.h"


struct Triangle
{
	Vector3D point1;
	Vector3D point2;
	Vector3D point3;

	bool IsPointInCircumference(const Vector3D& Point)
	{
		Vector2D XRange;
		Vector2D YRange;
		Vector2D ZRange;

		if (point1 > )


		float circumference = 
	}
};

class DelaunayTriangulation
{
public:

	void Triangulate(Array<Vector3D>& Vertices, Array<size_t>& Indicies);

private:

	Triangle GetSuperTriangle(const Array<Vector3D>& Vertices);

};

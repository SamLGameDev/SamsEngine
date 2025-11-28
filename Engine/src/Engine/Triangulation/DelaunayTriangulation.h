
#pragma once

#include "ErrorCodes.h"
#include "SubsystemInitialiser.h"


struct Circle
{
	Vector3D Pos;
	float Radius;

	Circle(const Vector3D& InPos, const float& InRadius) : Pos(InPos), Radius(InRadius) {};

	Circle(const Vector3D& P1, const Vector3D& P2);

	Circle(const Vector3D& P1, const Vector3D& P2, const Vector3D& P3);

	Vector3D GetCircleCenter(const Vector3D& AToB, const Vector3D& AToC);

	bool ArePointsInsideCircle(const Array<Vector3D>& Points);

	bool IsPointInsideCircle(const Vector3D& Point);

};

struct Triangle
{
	Vector3D point1;
	Vector3D point2;
	Vector3D point3;

	Circle GetMinCircleTrivial(Array<Vector3D>& EdgeRPoints);

	Circle GetSmallestCircle(Array<Vector3D>& Points, Array<Vector3D>& EdgeRPoints);

	bool IsPointInCircumference(const Vector3D& Point);
};

class DelaunayTriangulation
{
public:

	void Triangulate(Array<Vector3D>& Vertices, Array<size_t>& Indicies);

private:

	Triangle GetSuperTriangle(const Array<Vector3D>& Vertices);

};

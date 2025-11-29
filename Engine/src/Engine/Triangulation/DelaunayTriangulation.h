
#pragma once

#include "ErrorCodes.h"
#include "SubsystemInitialiser.h"


struct Edge
{
	Vector3D P1;
	Vector3D P2;
	Edge(const Vector3D& InP1, const Vector3D& InP2) : P1(InP1), P2(InP2) {};

	Edge() : P1(Vector3D::Zero), P2(Vector3D::Zero) {};

	bool operator==(const Edge& other) const
	{
		return (P1 == other.P1 && P2 == other.P2) || (P1 == other.P2 && P2 == other.P1);
	}
};


struct Circle
{
	Vector3D Pos;
	float Radius;

	Circle(const Vector3D& InPos, const float& InRadius) : Pos(InPos), Radius(InRadius) {};

	Circle(const Vector3D& P1, const Vector3D& P2);

	Circle(const Vector3D& P1, const Vector3D& P2, const Vector3D& P3);

	Vector3D GetCircleCenter(const Vector3D& AToB, const Vector3D& AToC);

	bool ArePointsInsideCircle(const Array<Vector3D>& Points)const;

	bool IsPointInsideCircle(const Vector3D& Point) const;

};

struct Triangle
{
	Vector3D point1;
	Vector3D point2;
	Vector3D point3;

	Circle GetMinCircleTrivial(Array<Vector3D>& EdgeRPoints) const;

	Circle GetSmallestCircle(Array<Vector3D>& Points, Array<Vector3D>& EdgeRPoints, const size_t& Size) const;

	bool IsPointInCircumference(const Vector3D& Point) const;
};

class DelaunayTriangulation
{
public:

	void Triangulate(Array<Vector3D>& Vertices, Array<size_t>& Indicies);

private:

	Triangle GetSuperTriangle(const Array<Vector3D>& Vertices);

};

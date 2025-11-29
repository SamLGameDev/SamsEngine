
#pragma once

#include "ErrorCodes.h"
#include "SubsystemInitialiser.h"


struct Edge
{
	Vector2D P1;
	Vector2D P2;
	Edge(const Vector2D& InP1, const Vector2D& InP2) : P1(InP1), P2(InP2) {};

	Edge() : P1(Vector2D::Zero), P2(Vector2D::Zero) {};

	bool operator==(const Edge& other) const
	{
		return (P1 == other.P1 && P2 == other.P2) || (P1 == other.P2 && P2 == other.P1);
	}

};

struct EquationLine
{
	float A;
	float B;
	float C;
	EquationLine() = default;

	EquationLine(const Vector2D& InA, const Vector2D& InB);

	EquationLine PerpendicularLineAt(const Vector2D& Pos);
};


struct Circle
{
	Vector2D Pos;
	float Radius;

	Circle(const Vector2D& InPos, const float& InRadius) : Pos(InPos), Radius(InRadius) {};

	Circle(const Vector2D& P1, const Vector2D& P2);

	Circle(const Vector2D& P1, const Vector2D& P2, const Vector2D& P3);

	Vector2D GetCircleCenter(const Vector2D& AToB, const Vector2D& AToC);

	bool ArePointsInsideCircle(const Array<Vector2D>& Points)const;

	bool IsPointInsideCircle(const Vector2D& Point) const;

};

struct Triangle
{
	Vector2D point1;
	Vector2D point2;
	Vector2D point3;

	Circle GetMinCircleTrivial(Array<Vector2D>& EdgeRPoints) const;

	Circle GetSmallestCircle(Array<Vector2D>& Points, Array<Vector2D> EdgeRPoints, const size_t& Size) const;

	bool IsPointInCircumference(const Vector2D& Point) const;
};

class DelaunayTriangulation
{
public:

	void Triangulate(Array<Vector2D>& Vertices, Array<size_t>& Indicies);

private:

	Triangle GetSuperTriangle(const Array<Vector2D>& Vertices);

};

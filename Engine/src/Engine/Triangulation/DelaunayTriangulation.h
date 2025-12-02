
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

struct Tetrahedron
{
	Vector3D point1;
	Vector3D point2;
	Vector3D point3;
	Vector3D point4;

	Face faces[4];

	bool IsPointInCircumSphere(const Vector3D& Point) const;

	Tetrahedron(const Vector3D& InP1, const Vector3D& InP2, const Vector3D& InP3, const Vector3D& InP4) :
		point1(InP1), point2(InP2), point3(InP3), point4(InP4)
	{
		faces[0] = { {InP1, InP2, InP3} };
		faces[1] = { {InP1, InP2, InP4} };
		faces[2] = { {InP1, InP3, InP4} };
		faces[3] = { {InP2, InP3, InP4} };
	};

	Tetrahedron() = default;
};

class DelaunayTriangulation
{
public:

	void Triangulate(Array<Vector2D>& Vertices, Array<uint16_t>& Indicies);
	void Triangulate(Array<Vector3D>& Vertices, Array<uint16_t>& Indicies);

private:

	Triangle GetSuperTriangle(const Array<Vector2D>& Vertices);
	Tetrahedron GetSuperTetrahedron(const Array<Vector3D>& Vertices);

};

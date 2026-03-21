// DO NOT MARK 
//This is because it has been submitted for my COMP305. Link to Original: https://github.falmouth.ac.uk/GA-Undergrad-Student-Work-25-26/Comp305-Engine-SL295211.git
#pragma once
#include "Array.h"
#include "Vector2D.h"


struct EquationLine
{
	float A;
	float B;
	float C;
	EquationLine() = default;

	EquationLine(const Vector2D& InA, const Vector2D& InB);

	EquationLine PerpendicularLineAt(const Vector2D& Pos) const;
};


struct Circle
{
	Vector2D Pos;
	float Radius;

	Circle(const Vector2D& InPos, const float& InRadius) : Pos(InPos), Radius(InRadius) {};

	Circle(const Vector2D& P1, const Vector2D& P2);

	Circle(const Vector2D& P1, const Vector2D& P2, const Vector2D& P3);

	static Vector2D GetCircleCenter(const Vector2D& AToB, const Vector2D& AToC);

	bool ArePointsInsideCircle(const Array<Vector2D>& Points)const;

	bool IsPointInsideCircle(const Vector2D& Point) const;

	static Circle GetMinCircleTrivial(const Array<Vector2D>& EdgeRPoints);

	static Circle GetSmallestCircle(Array<Vector2D>& Points, Array<Vector2D> EdgeRPoints, const size_t& Size);

};

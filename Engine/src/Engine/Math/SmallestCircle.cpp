// DO NOT MARK 
//This is because it has been submitted for my COMP305. Link to Original: https://github.falmouth.ac.uk/GA-Undergrad-Student-Work-25-26/Comp305-Engine-SL295211.git
#include "SmallestCircle.h"

#include <algorithm>
#include <cassert>

#include "MathCore.h"

EquationLine::EquationLine(const Vector2D& InA, const Vector2D& InB)
{
	Vector2D delta = InB - InA;
	A = delta.Y;
	B = -delta.X;
	C = A * InA.X + B * InA.Y;
}

EquationLine EquationLine::PerpendicularLineAt(const Vector2D& Pos) const
{
	EquationLine line;
	line.A = -B;
	line.B = A;
	line.C = line.A * Pos.X + line.B * Pos.Y;
	return line;
}

Circle::Circle(const Vector2D& P1, const Vector2D& P2)
{
	Pos = (P1 + P2) / 2;

	Radius = ((P1 - P2) / 2).GetLength();

}

Circle::Circle(const Vector2D& P1, const Vector2D& P2, const Vector2D& P3)
{
	Pos = P1 + GetCircleCenter(P2 - P1, P3 - P1);

	Radius = (Pos - P1).GetLength();
}

Vector2D Circle::GetCircleCenter(const Vector2D& AToB, const Vector2D& AToC)
{

	const double b = AToB.GetSquaredLength();
	const double c = AToC.GetSquaredLength();

	const float d = 2 * Vector2D::Cross(AToB, AToC);

	return { (AToC.Y * b - AToB.Y * c) / d, (AToB.X * c - AToC.X * b) / d };
}

bool Circle::ArePointsInsideCircle(const Array<Vector2D>& Points) const
{
	return std::ranges::all_of(Points, [this](const Vector2D& p)
		{
			return IsPointInsideCircle(p);
		});
}

bool Circle::IsPointInsideCircle(const Vector2D& Point) const
{
	const float length = (Point - Pos).GetSquaredLength();
	const float slength = (Radius * Radius);

	if (length <= slength) return true;

	return false;
}
Circle Circle::GetMinCircleTrivial(const Array<Vector2D>& EdgeRPoints)
{
	assert(EdgeRPoints.GetSize() <= 3);

	if (EdgeRPoints.IsEmpty()) return { Vector2D::Zero, 0 };

	if (EdgeRPoints.GetSize() == 1) return { EdgeRPoints[0], 0 };

	if (EdgeRPoints.GetSize() == 2) return { EdgeRPoints[0], EdgeRPoints[1] };

	for (size_t i = 0; i < 2; i++)
	{
		for (size_t j = i + 1; j < 3; j++)
		{
			Circle c = { EdgeRPoints[i], EdgeRPoints[j] };
			if (c.ArePointsInsideCircle(EdgeRPoints)) return c;
		}
	}
	return { EdgeRPoints[0], EdgeRPoints[1], EdgeRPoints[2] };
}

Circle Circle::GetSmallestCircle(Array<Vector2D>& Points, Array<Vector2D> EdgeRPoints, const size_t& Size)
{
	if (Size == 0 || EdgeRPoints.GetSize() == 3) return GetMinCircleTrivial(EdgeRPoints);

	size_t index = MathCore::RandomRange<int>(0, static_cast<int>(Size) - 1);
	Vector2D point = Points[index];

	Points.Swap(index, Size - 1);

	Circle c = GetSmallestCircle(Points, EdgeRPoints, Size - 1);

	if (c.IsPointInsideCircle(point)) return c;

	EdgeRPoints.Add(point);

	c = GetSmallestCircle(Points, EdgeRPoints, Size - 1);

	return c;

}

#include "DelaunayTriangulation.h"

#include <algorithm>
#include "Camera.h"
#include "MathCore.h"

void DelaunayTriangulation::Triangulate(Array<Vector3D>& Vertices, Array<size_t>& Indicies)
{
	Triangle superTriangle = GetSuperTriangle(Vertices);

	Array<Triangle> Triangles = { superTriangle };

	for (const auto& point : Vertices)
	{
		for (const auto& triangle : Triangles)
		{
			//Is Point inside triangle?
			//Connect to triangle
			//Remove Super triangle rsulting triangles?? Hwow
			//if triangle contains super triangle point?
			/// too inefficnet?
			///Seperate triangulation from Indexing? 
			/// Use the old method? Maps? Better method?}
			/// //Each triangle point has an associated index? tah twe assign
		}
	}


}

Triangle DelaunayTriangulation::GetSuperTriangle(const Array<Vector3D>& Vertices)
{
	Vector2D XRange = { std::numeric_limits<float>::max(), std::numeric_limits<float>::min() };
	Vector2D YRange = { std::numeric_limits<float>::max(), std::numeric_limits<float>::min() };
	Vector2D ZRange = { std::numeric_limits<float>::max(), std::numeric_limits<float>::min() };

	for(const auto& vertex : Vertices)
	{
		XRange.X = std::min(vertex.X, XRange.X);
		XRange.Y = std::max(vertex.X, XRange.Y);
		YRange.X = std::min(vertex.Y, YRange.X);
		YRange.Y = std::max(vertex.Y, YRange.Y);
		ZRange.X = std::min(vertex.Z, ZRange.X);
		ZRange.Y = std::max(vertex.Z, ZRange.Y);
	}

	const float diffX = XRange.Y - XRange.X;
	const float diffZ = ZRange.Y - ZRange.X;


	return { {XRange.X, YRange.X, ZRange.X}, {XRange.X + (diffX / 2), YRange.Y, ZRange.X + (diffZ /2)}, {XRange.Y, YRange.X, ZRange.Y}};
}

Circle Triangle::GetMinCircleTrivial(Array<Vector3D>& EdgeRPoints)
{
	assert(EdgeRPoints.GetSize() <= 3);

	if (EdgeRPoints.IsEmpty()) return { Vector3D::Zero, 0 };

	if (EdgeRPoints.GetSize() == 1) return { EdgeRPoints[0], 0 };

	if (EdgeRPoints.GetSize() == 2) return { EdgeRPoints[0], EdgeRPoints[1] };

	for (size_t i = 0; i < 2; i++)
	{
		for (size_t j = i+1; j < 3; j++)
		{
			Circle c = { EdgeRPoints[i], EdgeRPoints[j] };
			if (c.ArePointsInsideCircle(EdgeRPoints)) return c;
		}
	}
	return { EdgeRPoints[0], EdgeRPoints[1], EdgeRPoints[2] };
}

Circle Triangle::GetSmallestCircle(Array<Vector3D>& Points, Array<Vector3D>& EdgeRPoints, const size_t& Size)
{
	if (Size == 0 || EdgeRPoints.GetSize() == 3) return GetMinCircleTrivial(EdgeRPoints);

	size_t index = MathCore::RandomRange<int>(0, Size - 1);
	Vector3D point = Points[index];

	Points.Swap(index, Size - 1);

	Circle c = GetSmallestCircle(Points, EdgeRPoints, Size - 1);

	if (c.IsPointInsideCircle(point)) return c;

	EdgeRPoints.Add(point);

	c = GetSmallestCircle(Points, EdgeRPoints, Size - 1);

	return c;

}

bool Triangle::IsPointInCircumference(const Vector3D& Point)
{
	Array<Vector3D> Points = { point1, point2, point3 };
	Array<Vector3D> EdgeRPoints;

	const Circle c = GetSmallestCircle(Points, EdgeRPoints, Points.GetSize());

	if (c.IsPointInsideCircle(Point)) return true;

	return false;
}

Circle::Circle(const Vector3D& P1, const Vector3D& P2)
{
	Pos = (P1 + P2) / 2;

	Radius = ((P1 - P2) / 2).GetLength();

}

Circle::Circle(const Vector3D& P1, const Vector3D& P2, const Vector3D& P3)
{
	Pos = P1 + GetCircleCenter(P2 - P1, P3 - P1);

	Radius = (Pos - P1).GetLength();
}

Vector3D Circle::GetCircleCenter(const Vector3D& AToB, const Vector3D& AToC)
{
	const double b = AToB.GetSquaredLength();
	const double c = AToC.GetSquaredLength();

	const Vector3D d = Vector3D::Cross(AToB, AToC);

	return (Vector3D::Dot(AToB, AToB) * Vector3D::Cross(AToC, d) + Vector3D::Dot(AToC, AToC) * Vector3D::Cross(d, AToB)) / (2 * Vector3D::Dot(d, d));
}

bool Circle::ArePointsInsideCircle(const Array<Vector3D>& Points) const
{
	return std::ranges::all_of(Points, [this](const Vector3D& p)
	{
		return IsPointInsideCircle(p);
	});
}

bool Circle::IsPointInsideCircle(const Vector3D& Point) const
{
	if ((Point - Pos).GetSquaredLength() <= (Radius * Radius)) return true;

	return false;
}

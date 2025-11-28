#include "DelaunayTriangulation.h"
#include "Camera.h"

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
		if (vertex.X < XRange.X)
		{
			XRange.X = vertex.X;
		}
		if (vertex.X > XRange.Y)
		{
			XRange.Y = vertex.X;
		}
		if (vertex.Y < YRange.X)
		{
			YRange.X = vertex.Y;
		}
		if (vertex.Y > YRange.Y)
		{
			YRange.Y = vertex.Y;
		}
		if (vertex.Z < ZRange.X)
		{
			ZRange.X = vertex.Z;
		}
		if (vertex.Z > ZRange.Y)
		{
			ZRange.Y = vertex.Z;
		}
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

Circle Triangle::GetSmallestCircle(Array<Vector3D>& Points, Array<Vector3D>& EdgeRPoints, )
{
	if (Points.IsEmpty() || EdgeRPoints.GetSize() == 3) return GetMinCircleTrivial(EdgeRPoints);


}

bool Triangle::IsPointInCircumference(const Vector3D& Point)
{
	Array<Vector3D> Points = { point1, point2, point3 };
	Array<Vector3D> EdgeRPoints;




		float circumference =

	return false;
}

Circle::Circle(const Vector3D& P1, const Vector3D& P2)
{
	Pos = (P1 + P2) / 2;

	Radius = ((P1 - P2) / 2).GetLength();

}

Circle::Circle(const Vector3D& P1, const Vector3D& P2, const Vector3D& P3)
{
	Pos = GetCircleCenter(P2 - P1, P3 - P1);

	Radius = (Pos - P1).GetLength();
}

Vector3D Circle::GetCircleCenter(const Vector3D& AToB, const Vector3D& AToC)
{
	const double b = AToB.GetSquaredLength();
	const double c = AToC.GetSquaredLength();

	const Vector3D d = Vector3D::Cross(AToB, AToC);

	return (Vector3D::Dot(AToB, AToB) * Vector3D::Cross(AToC, d) + Vector3D::Dot(AToC, AToC) * Vector3D::Cross(d, AToB)) / (2 * Vector3D::Dot(d, d));
}

bool Circle::ArePointsInsideCircle(const Array<Vector3D>& Points)
{
	for (const auto& p : Points)
	{
		if (IsPointInsideCircle(p)) continue;
		return false;
	}

	return true;
}

bool Circle::IsPointInsideCircle(const Vector3D& Point)
{
	if ((Point - Pos).GetSquaredLength() <= (Radius * Radius)) return true;

	return false;
}

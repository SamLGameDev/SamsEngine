#include "DelaunayTriangulation.h"

#include <algorithm>
#include "Camera.h"
#include "MathCore.h"
#include "Matrix.h"

bool Tetrahedron::IsPointInCircumSphere(const Vector3D& Point) const
{

	Matrix<4, 4> sign = {
		point1.X, point1.Y, point1.Z, 1,
		point2.X, point2.Y, point2.Z, 1,
		point3.X, point3.Y, point3.Z,1,
		point4.X, point4.Y, point4.Z, 1
	};

	const float signDet = sign.CalculateDeterminant();

	if (signDet == 0) return false;


	Matrix<5, 5> mat = 
	{
		point1.X, point1.Y, point1.Z, (point1.X * point1.X) + (point1.Y * point1.Y) + (point1.Z * point1.Z),1.0f,
		point2.X, point2.Y, point2.Z, (point2.X * point2.X) + (point2.Y * point2.Y) + (point2.Z * point2.Z),1.0f,
		point3.X, point3.Y, point3.Z, (point3.X * point3.X) + (point3.Y * point3.Y) + (point3.Z * point3.Z),1.0f,
		point4.X, point4.Y, point4.Z, (point4.X * point4.X) + (point4.Y * point4.Y) + (point4.Z * point4.Z),1.0f,
		Point.X,  Point.Y,   Point.Z,   (Point.X * Point.X) + (Point.Y * Point.Y) + (Point.Z * Point.Z),  1.0f
		
	};

	const float deternminate = mat.CalculateDeterminant();

	return signDet > 0 ? deternminate > 0 : deternminate < 0;
}

void DelaunayTriangulation::Triangulate(Array<Vector2D>& Vertices, Array<uint16_t>& Indicies)
{
	Triangle superTriangle = GetSuperTriangle(Vertices);

	Array<Triangle> Triangles = { superTriangle };

	for (const auto& point : Vertices)
	{
		Array<Triangle> NewTriangles;

		Array<Edge> Edges;

		for (const auto& triangle : Triangles)
		{

			if (!triangle.IsPointInCircumference(point)) {
				NewTriangles.Add(triangle);
				continue;
			}
			Edges.Add({ triangle.point1, triangle.point2 });
			Edges.Add({ triangle.point2, triangle.point3 });
			Edges.Add({ triangle.point3, triangle.point1});
		}


		Array<Edge> uniqueEdges;

		for (size_t i = 0; i < Edges.GetSize(); i++)
		{
			bool unique = true;

			for (size_t j = 0; j < Edges.GetSize(); j++)
			{
				if (i == j) continue;

				if (Edges[i] == Edges[j])
				{
					unique = false;
					break;
				}
			}

			if (unique)
			{
				uniqueEdges.Add(Edges[i]);
			}
		}

		for (const auto& edge : uniqueEdges)
		{
			NewTriangles.Add({ edge.P1, edge.P2, point });
		}


		Triangles = NewTriangles;

	}

	for (const auto& triangle : Triangles)
	{
		if (triangle.point1 == superTriangle.point1 || triangle.point1 == superTriangle.point2 || triangle.point1 == superTriangle.point3) continue;
		if (triangle.point2 == superTriangle.point1 || triangle.point2 == superTriangle.point2 || triangle.point2 == superTriangle.point3) continue;
		if (triangle.point3 == superTriangle.point1 || triangle.point3 == superTriangle.point2 || triangle.point3 == superTriangle.point3) continue;
		size_t index1 = 0;
		size_t index2 = 0;
		size_t index3 = 0;
		for (size_t i = 0; i < Vertices.GetSize(); i++)
		{
			if (Vertices[i] == triangle.point1) index1 = i;
			if (Vertices[i] == triangle.point2) index2 = i;
			if (Vertices[i] == triangle.point3) index3 = i;
		}
		Indicies.Add(index1);
		Indicies.Add(index2);
		Indicies.Add(index3);
	}

}

void DelaunayTriangulation::Triangulate(Array<Vector3D>& Vertices, Array<uint16_t>& Indicies)
{
	Tetrahedron superTetrahedron = GetSuperTetrahedron(Vertices);

	Array<Tetrahedron> tetrahedra = { superTetrahedron };

	for (const auto& point : Vertices)
	{
		Array<Tetrahedron> newTetrahedron;

		Array<Face> faces;

		for (const auto& tetrahedron : tetrahedra)
		{

 			if (!tetrahedron.IsPointInCircumSphere(point)) {
				newTetrahedron.Add(tetrahedron);
				continue;
			}
			for (const auto& face : tetrahedron.faces)
			{
				faces.Add(face);
			}
		}

		Array<Face> boundaryFaces;
		Array<Face> appearedFaces;
		for (int i = 0; i < faces.GetSize(); ++i)
		{
			if (!appearedFaces.Contains(faces[i]))
			{
				if (boundaryFaces.Contains(faces[i]))
				{
					boundaryFaces.Remove(faces[i]);
					appearedFaces.Add(faces[i]);
				}
				else
				{
					boundaryFaces.Add(faces[i]);
				}
			}
		}

		if (boundaryFaces.IsEmpty()) {
			tetrahedra = newTetrahedron; continue;
		}


		for (const auto& f: boundaryFaces)
		{
			newTetrahedron.Add(Tetrahedron(f.Vertices[0], f.Vertices[1], f.Vertices[2], point));
		}
		tetrahedra = newTetrahedron;
	}

	Vertices.Empty();

	for (const auto& tet : tetrahedra)
	{
		//if (tet.point1 == superTetrahedron.point1 || tet.point1 == superTetrahedron.point2 || tet.point1 == superTetrahedron.point3 || tet.point1 == superTetrahedron.point4) continue;
		//if (tet.point2 == superTetrahedron.point1 || tet.point2 == superTetrahedron.point2 || tet.point2 == superTetrahedron.point3 || tet.point2 == superTetrahedron.point4) continue;
		//if (tet.point3 == superTetrahedron.point1 || tet.point3 == superTetrahedron.point2 || tet.point3 == superTetrahedron.point3 || tet.point3 == superTetrahedron.point4) continue;
		//if (tet.point4 == superTetrahedron.point1 || tet.point4 == superTetrahedron.point2 || tet.point4 == superTetrahedron.point3 || tet.point4 == superTetrahedron.point4) continue;

		for (const auto& face : tet.faces)
		{
			bool skipFace = false;

			for (const auto& verts : face.Vertices)
			{
				if (verts == superTetrahedron.point1 || verts == superTetrahedron.point2 || verts == superTetrahedron.point3 || verts == superTetrahedron.point4)
				{
					skipFace = true;
					break;
				}
			}
			
			if (skipFace) continue;

			for (size_t t= 1; t < 3; t++)
			{

				unsigned int index = 0;

				if (Vertices.Contains(face.Vertices[0], index))
				{
					Indicies.Add(index);
				}
				else
				{

					Vertices.Add(face.Vertices[t]);
					Indicies.Add(Vertices.GetSize() - 1);
				}

				if (Vertices.Contains(face.Vertices[t], index))
				{
					Indicies.Add(index);
				}
				else
				{
					Vertices.Add(face.Vertices[t]);
					Indicies.Add(Vertices.GetSize() - 1);
				}
				if (Vertices.Contains(face.Vertices[(t + 1) % 3], index))
				{
					Indicies.Add(index);
				}
				else
				{
					Vertices.Add(face.Vertices[(t + 1) % 3]);
					Indicies.Add(Vertices.GetSize() - 1);
				}
			}
		}
	}
}

Triangle DelaunayTriangulation::GetSuperTriangle(const Array<Vector2D>& Vertices)
{
	Vector2D XRange = { std::numeric_limits<float>::max(), std::numeric_limits<float>::min() };
	Vector2D YRange = { std::numeric_limits<float>::max(), std::numeric_limits<float>::min() };

	for(const auto& vertex : Vertices)
	{
		XRange.X = std::min(vertex.X, XRange.X);
		XRange.Y = std::max(vertex.X, XRange.Y);
		YRange.X = std::min(vertex.Y, YRange.X);
		YRange.Y = std::max(vertex.Y, YRange.Y);

	}

	const float diffX = (XRange.Y - XRange.X)* 10;
	const float diffY = (YRange.Y - YRange.X) * 10;

	

	return { {XRange.X - diffX, YRange.X - diffY * 3} , {XRange.X - diffX, YRange.Y + diffY}, {XRange.Y + diffX * 3, YRange.Y + diffY} };
}

Tetrahedron DelaunayTriangulation::GetSuperTetrahedron(const Array<Vector3D>& Vertices)
{
	Vector3D min = { std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max() };
	Vector3D max = { std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest() };

	for (const auto& vertex : Vertices)
	{
		min.X = std::min(vertex.X,  min.X);
		max.X = std::max(vertex.X, max.X);
		min.Y = std::min(vertex.Y, min.Y);
		max.Y = std::max(vertex.Y, max.Y);
		min.Z = std::min(vertex.Z, min.Z);
		max.Z = std::max(vertex.Z, max.Z);

	}

	const Vector3D center = (min + max) / 2.0f;

	Vector3D BoundingBoxLength = max - min;

	const float radius = BoundingBoxLength.GetLength();

	float pad = std::max({ BoundingBoxLength.X, BoundingBoxLength.Y, BoundingBoxLength.Z });

	const float safeRadius = pad * 10.0f;

	Vector3D a = center + Vector3D(safeRadius * 2, safeRadius, safeRadius);
	Vector3D b = center + Vector3D(-(safeRadius * 2), -safeRadius, safeRadius);
	Vector3D c = center + Vector3D(-(2 *safeRadius), safeRadius, -safeRadius);
	Vector3D d = center + Vector3D(2 *safeRadius, -safeRadius, -safeRadius);


	return { a, b, c, d};
}

Circle Triangle::GetMinCircleTrivial(Array<Vector2D>& EdgeRPoints) const
{
	assert(EdgeRPoints.GetSize() <= 3);

	if (EdgeRPoints.IsEmpty()) return { Vector2D::Zero, 0 };

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

Circle Triangle::GetSmallestCircle(Array<Vector2D>& Points, Array<Vector2D> EdgeRPoints, const size_t& Size) const
{
	if (Size == 0 || EdgeRPoints.GetSize() == 3) return GetMinCircleTrivial(EdgeRPoints);

	size_t index = MathCore::RandomRange<int>(0, Size - 1);
	Vector2D point = Points[index];

	Points.Swap(index, Size - 1);

	Circle c = GetSmallestCircle(Points, EdgeRPoints, Size - 1);

	if (c.IsPointInsideCircle(point)) return c;

	EdgeRPoints.Add(point);

	c = GetSmallestCircle(Points, EdgeRPoints, Size - 1);

	return c;

}

bool Triangle::IsPointInCircumference(const Vector2D& Point) const
{
	//Array<Vector2D> Points = { point1, point2, point3 };
	//Array<Vector2D> EdgeRPoints;

	//const Circle c = GetSmallestCircle(Points, EdgeRPoints, Points.GetSize());

	//if (c.IsPointInsideCircle(Point)) return true;

	EquationLine LineAB = EquationLine(point1, point2);
	EquationLine LineBC = EquationLine(point2, point3);

	Vector2D midpointAB = Vector2D::Lerp(point1, point2, 0.5f);
	Vector2D midpointBC = Vector2D::Lerp(point2, point3, 0.5f);

	EquationLine perpendicularAB = LineAB.PerpendicularLineAt(midpointAB);
	EquationLine perpendicularBC = LineBC.PerpendicularLineAt(midpointBC);

	float determinant = perpendicularAB.A * perpendicularBC.B - perpendicularBC.A * perpendicularAB.B;
	float determinantX = perpendicularAB.C * perpendicularBC.B - perpendicularBC.C * perpendicularAB.B;
	float determinantY = perpendicularAB.A * perpendicularBC.C - perpendicularBC.A * perpendicularAB.C;

	Vector2D circle = {determinantX / determinant, determinantY / determinant};

	float radius = (point1 - circle).GetLength();

	Circle c = { circle, radius };

	return c.IsPointInsideCircle(Point);
}

EquationLine::EquationLine(const Vector2D& InA, const Vector2D& InB)
{
	Vector2D delta = InB - InA;
	A = delta.Y;
	B = -delta.X;
	C = A * InA.X + B * InA.Y;
}

EquationLine EquationLine::PerpendicularLineAt(const Vector2D& Pos)
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
	float length = (Point - Pos).GetSquaredLength();
	float slength = (Radius * Radius);

	if (length <= slength) return true;

	return false;
}

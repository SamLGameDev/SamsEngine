// DO NOT MARK 
//This is because it has been submitted for my COMP305. Link to Original: https://github.falmouth.ac.uk/GA-Undergrad-Student-Work-25-26/Comp305-Engine-SL295211.git
#include "DelaunayTriangulation.h"

#include <algorithm>
#include "Camera.h"
#include "MathCore.h"
#include "Matrix.h"

bool Tetrahedron::IsPointInCircumSphere(const Vector3D& Point) const
{
	//Calculate orientation, then determinant
	Matrix<4, 4> sign = {
		point1.X, point1.Y, point1.Z, 1,
		point2.X, point2.Y, point2.Z, 1,
		point3.X, point3.Y, point3.Z,1,
		point4.X, point4.Y, point4.Z, 1
	};

	const float signDet = sign.CalculateDeterminant();

	if (MathCore::IsNearlyZero(signDet)) return false;

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

void DelaunayTriangulation::GetTrianglesWithCircumcirclesContainingPoint(const Array<Triangle>& Triangles, const Vector2D& Point, Array<Triangle>& NewTriangles, Array<Edge>& Edges)
{
	for (const auto& triangle : Triangles)
	{

		if (!triangle.IsPointInCircumference(Point)) {
			NewTriangles.Add(triangle);
			continue;
		}
		Edges.Add({ triangle.point1, triangle.point2 });
		Edges.Add({ triangle.point2, triangle.point3 });
		Edges.Add({ triangle.point3, triangle.point1});
	}
}

void DelaunayTriangulation::GetUniqueEdges(const Array<Edge>& Edges, Array<Edge>& UniqueEdges)
{
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
			UniqueEdges.Add(Edges[i]);
		}
	}
}

void DelaunayTriangulation::RemoveSuperTriangleAndIndex(const Array<Vector2D>& Vertices, Array<uint16_t>& Indicies, const Triangle& SuperTriangle, const Array<Triangle>&
                                                        Triangles)
{
	for (const auto& triangle : Triangles)
	{
		if (triangle.point1 == SuperTriangle.point1 || triangle.point1 == SuperTriangle.point2 || triangle.point1 == SuperTriangle.point3) continue;
		if (triangle.point2 == SuperTriangle.point1 || triangle.point2 == SuperTriangle.point2 || triangle.point2 == SuperTriangle.point3) continue;
		if (triangle.point3 == SuperTriangle.point1 || triangle.point3 == SuperTriangle.point2 || triangle.point3 == SuperTriangle.point3) continue;
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

void DelaunayTriangulation::Triangulate(const Array<Vector2D>& Vertices, Array<uint16_t>& Indicies)
{
	Triangle superTriangle = GetSuperTriangle(Vertices);

	Array<Triangle> triangles = { superTriangle };

	for (const auto& point : Vertices)
	{
		Array<Triangle> newTriangles;

		Array<Edge> edges;

		GetTrianglesWithCircumcirclesContainingPoint(triangles, point, newTriangles, edges);

		Array<Edge> uniqueEdges;

		GetUniqueEdges(edges, uniqueEdges);

		for (const auto& edge : uniqueEdges)
		{
			newTriangles.Add({ edge.P1, edge.P2, point });
		}

		triangles = newTriangles;

	}

	RemoveSuperTriangleAndIndex(Vertices, Indicies, superTriangle, triangles);

}

void DelaunayTriangulation::GetTetsWithPointInCircumsphere(const Array<Tetrahedron>& Tetrahedra, const Vector3D& Point, Array<Tetrahedron>& NewTetrahedron, Array<Face>& Faces)
{
	for (const auto& tetrahedron : Tetrahedra)
	{

		if (!tetrahedron.IsPointInCircumSphere(Point)) {
			NewTetrahedron.Add(tetrahedron);
			continue;
		}
		for (const auto& face : tetrahedron.faces)
		{
			Faces.Add(face);
		}
	}
}

void DelaunayTriangulation::GetUniqueFaces(const Array<Face>& Faces, Array<Face>& UniqueFaces)
{
	Array<Face> appearedFaces;
	for (size_t i = 0; i < Faces.GetSize(); i++)
	{
		if (!appearedFaces.Contains(Faces[i]))
		{
			size_t index;
			if (UniqueFaces.Contains(Faces[i], index))
			{
				UniqueFaces.RemoveAt(index);
				appearedFaces.Add(Faces[i]);
			}
			else
			{
				UniqueFaces.Add(Faces[i]);
			}
		}
	}
}

void DelaunayTriangulation::Triangulate(const Array<Vector3D>& Vertices)
{
	SuperTetrahedron = GetSuperTetrahedron(Vertices);

    Tetrahedrons = { SuperTetrahedron };

	for (const auto& point : Vertices)
	{
		Array<Tetrahedron> newTetrahedron;

		Array<Face> faces;

		GetTetsWithPointInCircumsphere(Tetrahedrons, point, newTetrahedron, faces);

		Array<Face> uniqueFaces;
		GetUniqueFaces(faces, uniqueFaces);

		if (uniqueFaces.IsEmpty())
		{
			Tetrahedrons = newTetrahedron;
			continue;
		}

		for (const auto& f: uniqueFaces)
		{
			newTetrahedron.Add(Tetrahedron(f.Vertices[0], f.Vertices[1], f.Vertices[2], point));
		}
		Tetrahedrons = newTetrahedron;
	}



	//Dont remove super triangle, as voronoi needs it to clip to box without adding box bounding points
}

void DelaunayTriangulation::RemoveSuperTriangle()
{
	Array<Tetrahedron> unique;
	for (const auto& tetrahedron : Tetrahedrons)
	{
		if (tetrahedron.ContainsPoint(SuperTetrahedron.point1)
			|| tetrahedron.ContainsPoint(SuperTetrahedron.point2)
			|| tetrahedron.ContainsPoint(SuperTetrahedron.point3)
			|| tetrahedron.ContainsPoint(SuperTetrahedron.point4))
		{
			continue;
		}
		unique.Add(tetrahedron);
	}
	Tetrahedrons = unique;
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

	return { {XRange.X - diffX, YRange.X - diffY * 3} ,
		{XRange.X - diffX, YRange.Y + diffY},
		{XRange.Y + diffX * 3, YRange.Y + diffY} };
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

	const Vector3D BoundingBoxLength = max - min;

	const float radius = BoundingBoxLength.GetLength();

	const float pad = std::max({ BoundingBoxLength.X, BoundingBoxLength.Y, BoundingBoxLength.Z });

	const float safeRadius = pad * 10.0f;

	const Vector3D a = center + Vector3D(safeRadius * 2, safeRadius, safeRadius);
	const Vector3D b = center + Vector3D(-(safeRadius * 2), -safeRadius, safeRadius);
	const Vector3D c = center + Vector3D(-(2 *safeRadius), safeRadius, -safeRadius);
	const Vector3D d = center + Vector3D(2 *safeRadius, -safeRadius, -safeRadius);


	return { a, b, c, d};
}


bool Triangle::IsPointInCircumference(const Vector2D& Point) const
{

	const EquationLine LineAB = EquationLine(point1, point2);
	const EquationLine LineBC = EquationLine(point2, point3);

	const Vector2D midpointAB = Vector2D::Lerp(point1, point2, 0.5f);
	const Vector2D midpointBC = Vector2D::Lerp(point2, point3, 0.5f);

	const EquationLine perpendicularAB = LineAB.PerpendicularLineAt(midpointAB);
	const EquationLine perpendicularBC = LineBC.PerpendicularLineAt(midpointBC);

	const float determinant = perpendicularAB.A * perpendicularBC.B - perpendicularBC.A * perpendicularAB.B;
	const float determinantX = perpendicularAB.C * perpendicularBC.B - perpendicularBC.C * perpendicularAB.B;
	const float determinantY = perpendicularAB.A * perpendicularBC.C - perpendicularBC.A * perpendicularAB.C;

	const Vector2D circle = {determinantX / determinant, determinantY / determinant};

	const float radius = (point1 - circle).GetLength();

	const Circle c = { circle, radius };

	return c.IsPointInsideCircle(Point);
}


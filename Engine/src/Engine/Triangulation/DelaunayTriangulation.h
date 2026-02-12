
#pragma once

#include <stack>

#include "ErrorCodes.h"
#include "SmallestCircle.h"
#include "SubsystemInitialiser.h"



struct Triangle
{
	Vector2D point1;
	Vector2D point2;
	Vector2D point3;

	bool IsPointInCircumference(const Vector2D& Point) const;
};


class DelaunayTriangulation
{
public:
	static void GetTrianglesWithCircumcirclesContainingPoint(const Array<Triangle>& Triangles, const Vector2D& Point,
	                                                         Array<Triangle>& NewTriangles, Array<Edge>& Edges);
	static void GetUniqueEdges(const Array<Edge>& Edges, Array<Edge>& UniqueEdges);
	static void RemoveSuperTriangleAndIndex(const Array<Vector2D>& Vertices, Array<uint16_t>& Indicies, const Triangle& SuperTriangle,
	                                        const Array<Triangle>& Triangles);
	static void Triangulate(const Array<Vector2D>& Vertices, Array<uint16_t>& Indicies);
	static void GetTetsWithPointInCircumsphere(const Array<Tetrahedron>& Tetrahedra, const Vector3D& Point,
	                                           Array<Tetrahedron>& NewTetrahedron, Array<Face>& Faces);
	static void GetUniqueFaces(const Array<Face>& Faces, Array<Face>& UniqueFaces);
	void Triangulate(const Array<Vector3D>& Points);

	void RemoveSuperTriangle();

	Array<Tetrahedron> Tetrahedrons;

	Tetrahedron SuperTetrahedron;

private:
	static Triangle GetSuperTriangle(const Array<Vector2D>& Vertices);
	static Tetrahedron GetSuperTetrahedron(const Array<Vector3D>& Vertices);

};

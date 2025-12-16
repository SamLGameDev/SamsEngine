
#include "Voronoi.h"
#include "transform.h"
#include <iostream>

#include "Camera.h"
#include "DataBuffers.h"
#include "DelaunayTriangulation.h"
#include "MathCore.h"
#include "ObjectFactory.h"
#include "InterfaceRenderer.h"
#include "glm/gtc/type_ptr.hpp"
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_3.h>
#include "Predictates.h"
using Kernel = CGAL::Exact_predicates_inexact_constructions_kernel;
using Point = Kernel::Point_3;
using DT = CGAL::Delaunay_triangulation_3<Kernel>;

void Voronoi::GetFirstIntersection(const Vector3D& Normal, const Vector3D& Center, const Face& CurrentFace, Face& NewFace, size_t& FirstIntersectionIndex, Vector3D& FirstIntersection)
{
	for (FirstIntersectionIndex; FirstIntersectionIndex < CurrentFace.Vertices.GetSize(); FirstIntersectionIndex++)
	{
		Vector3D fromVert = CurrentFace.Vertices[FirstIntersectionIndex];
		Vector3D toVert = CurrentFace.Vertices[(FirstIntersectionIndex + 1) % CurrentFace.Vertices.GetSize()];

		Vector3D lineDir = toVert - fromVert;
		Vector3D intersectionPoint;
		const bool bDoesLineIntersect = Vector3D::GetIntersectionPointWithPlane(Center, Normal, fromVert, lineDir, intersectionPoint);

		if (bDoesLineIntersect)
		{
			const bool intersectionIsNextVertex = intersectionPoint == toVert;

			if (intersectionIsNextVertex)
			{
				NewFace.Vertices.Add(toVert);
				NewFace.Vertices.Add(CurrentFace.Vertices[(FirstIntersectionIndex + 2) % CurrentFace.Vertices.GetSize()]);
				FirstIntersectionIndex = (FirstIntersectionIndex + 2) % CurrentFace.Vertices.GetSize();

			}
			else
			{
				NewFace.Vertices.Add(intersectionPoint);
				NewFace.Vertices.Add(toVert);
				FirstIntersectionIndex = (FirstIntersectionIndex + 1) % CurrentFace.Vertices.GetSize();
			}

			FirstIntersection = intersectionPoint;

			break;
						
		}

	}
}

size_t Voronoi::GetAllVertsUntilSecondIntersection(const Vector3D& Normal, const Vector3D& Center, const Face& CurrentFace, Face& NewFace, const size_t&
                                                   FirstIntersectionIndex, Vector3D& SecondIntersection)
{
	size_t secondIntersectionIndex = 0;

	for (secondIntersectionIndex = FirstIntersectionIndex; secondIntersectionIndex < CurrentFace.Vertices.GetSize(); secondIntersectionIndex++)
	{
		Vector3D fromVert = CurrentFace.Vertices[secondIntersectionIndex];
		Vector3D toVert = CurrentFace.Vertices[(secondIntersectionIndex + 1) % CurrentFace.Vertices.GetSize()];

		Vector3D lineDir = toVert - fromVert;
		Vector3D intersectionPoint;
		const bool bDoesLineIntersect = Vector3D::GetIntersectionPointWithPlane(Center, Normal, fromVert, lineDir, intersectionPoint);

		if (bDoesLineIntersect)
		{
			NewFace.Vertices.Add(intersectionPoint);
			SecondIntersection = intersectionPoint;
			secondIntersectionIndex++;
			break;

		}
		NewFace.Vertices.Add(toVert);
	}

	return secondIntersectionIndex;
}

void Voronoi::GetFaceReveresed(Face& IntersectFace, const Face& CurrentFace, Face& NewFace, const size_t& FirstIntersectionIndex, const Vector3D&
                               FirstIntersection, const Vector3D& SecondIntersection, const size_t& SecondIntersectionIndex)
{
	NewFace.Vertices.Empty();

	if (SecondIntersection != CurrentFace.Vertices[SecondIntersectionIndex % CurrentFace.Vertices.GetSize()]) NewFace.Vertices.Add(SecondIntersection);

	Vector3D from;

	for (size_t a = SecondIntersectionIndex; a % CurrentFace.Vertices.GetSize() != FirstIntersectionIndex; a++)
	{
		from = CurrentFace.Vertices[a % CurrentFace.Vertices.GetSize()];
		Vector3D to = CurrentFace.Vertices[(a + 1) % CurrentFace.Vertices.GetSize()];
		if (from == to) continue;

		NewFace.Vertices.Add(from);
	}

	if (from != FirstIntersection) NewFace.Vertices.Add(FirstIntersection);

	bool bAddFirst = true, bAddSecond = true;
	for (const auto& vert : IntersectFace.Vertices)
	{
		if (vert == FirstIntersection) bAddFirst = false;

		if (vert == SecondIntersection) bAddSecond = false;
	}



	if (bAddSecond) IntersectFace.Vertices.Add(SecondIntersection);
	if (bAddFirst) IntersectFace.Vertices.Add(FirstIntersection);


}

void Voronoi::SliceFaceByPlane(const Array<Face>& Faces, const Vector3D& Normal, const Vector3D& Center, Array<Face>& NewFaces,
                               Face& IntersectFace, const size_t& FaceIndex)
{
	const Face& currentFace = Faces[FaceIndex];
	Face newFace;

	size_t firstIntersectionIndex = 0;
	Vector3D firstIntersection;

	GetFirstIntersection(Normal, Center, currentFace, newFace, firstIntersectionIndex, firstIntersection);
		
	//Check if there is a first intersection, if not, 
	//check if face is on the current points side of the plane, and if so, keep it

	if (newFace.Vertices.IsEmpty())
	{
		if (Vector3D::Dot(Normal, currentFace.Vertices[0] - Center) > 0)
		{
			NewFaces.Add(currentFace);
		}
		return;
	}

	//Get all verts up to and including the second intersection

	Vector3D secondIntersection;
	size_t secondIntersectionIndex = GetAllVertsUntilSecondIntersection
	(
		Normal,
		Center,
		currentFace,
		newFace,
		firstIntersectionIndex,
		secondIntersection
	);

	//Is the point inside the polygon formed by the new face? if not, reverse it by
	//starting from the second intersection and continuing until the first intersection

	if (!IsPointInPolygon(Normal, newFace.Vertices, Center))
	{
		GetFaceReveresed
		(
			IntersectFace,
			currentFace,
			newFace,
			firstIntersectionIndex,
			firstIntersection,
			secondIntersection,
			secondIntersectionIndex
		);
	}
	else
	{

		// if the intersections arent already in the intersect face, add them

		bool bAddFirst = true, bAddSecond = true;
		for (const auto& vert : IntersectFace.Vertices)
		{
			if (vert == firstIntersection) bAddFirst = false;

			if (vert == secondIntersection) bAddSecond = false;
		}

		if (bAddFirst) IntersectFace.Vertices.Add(firstIntersection);

		if (bAddSecond) IntersectFace.Vertices.Add(secondIntersection);




	}

	NewFaces.Add(newFace);
}

void Voronoi::SliceShapeByPlane(const Array<Vector3D>& Points, const size_t& Index, Vector3D& CurrentPoint, Array<Face>& Faces,
                                Vector3D& Normal, Vector3D& Right, Vector3D& Up, Vector3D& Center, const size_t& J)
{
	if (J == Index)return;

	Vector3D comparedPoint = Points[J];

	Array<Face> newFaces;

	Face intersectFace;

	DefinePlane(Normal, CurrentPoint, comparedPoint, Right, Up, Center);

	//Slice each face by plane
	for (size_t f = 0; f < Faces.GetSize(); f++)
	{
		SliceFaceByPlane(Faces, Normal, Center, newFaces, intersectFace, f);
	}

	//if there is a valid intersect face, add it to the list of faces
	if (intersectFace.Vertices.GetSize() > 2)
	{
		size_t index = 0;
		float mostdot = -1.1f;
		Vector3D mostn;
		for (size_t i = 0; i <  newFaces.GetSize(); i++)
		{
			Vector3D normal = ComputePolygonNormal(newFaces[i].Vertices);

			if (float d = Vector3D::Dot(normal, -Normal) > mostdot)
			{
				index = i;
				mostdot = d;
				mostn = normal;
			}
		}

		Vector3D cross = Vector3D::Cross(mostn, Normal).Normalised();

		if (Vector3D::Dot(cross, -Right) < 0)
		{
			newFaces.Insert(intersectFace, index);
		}
		else
		{
			newFaces.Insert(intersectFace, index);
		}
	}

	Faces = newFaces;
}

Vector3D Voronoi::ComputePolygonNormal(const Array<Vector3D>& verts)
{
	Vector3D normal(0, 0, 0);

	size_t count = verts.GetSize();
	for (size_t i = 0; i < count; i++)
	{
		const Vector3D& current = verts[i];
		const Vector3D& next = verts[(i + 1) % count];

		normal.X += (current.Y - next.Y) * (current.Z - next.Z);
		normal.Y += (current.Z - next.Z) * (current.X - next.X);
		normal.Z += (current.X - next.X) * (current.Y - next.Y);
	}

	return normal.Normalised();
}

void Voronoi::FracturePlaneRandom(Model& InModel, const size_t& NumPoints)
{
	Array<Vector3D> points;
	GenerateRandomPointsInBounds(InModel, NumPoints, points);

	//TestPoints
	//points = { {0.8, 0.4, 0.2}, {0.4, 0.8, 0.6}, {0.1, 0.1, 0.1} };
	//points = { {0.1, 0.1, 0.1}, {0.8, 0.4, 0.2} };


	for (size_t i = 0; i < points.GetSize(); i++)
	{
		Vector3D currentPoint = points[i];

		Array<Face> Faces = InModel.BoundingBox->Faces;

		Vector3D normal, right, up, center;

		for (size_t j = 0; j < points.GetSize(); j++)
		{
			SliceShapeByPlane(points, i, currentPoint, Faces, normal, right, up, center, j);
		}

		auto color = Vector3D::RandomRange(Vector3D(30, 30, 30), Vector3D(255, 255, 255));


		//for (auto& face : Faces)
		//{
		//	FracturePiece3D* frac = CreateObjectPtr<FracturePiece3D>(face.Vertices, currentPoint);
		//	frac->color = color;
		//	fractureFaces.Add(face);
		//}
//
		Array<Face> test = { Faces };

		FracturePiece3D frac = CreateObjectRaw<FracturePiece3D>(test, currentPoint);
		frac.color = color;
		Fractures.Add({ frac });

	}

}

Array<Vector3D> Voronoi::GenerateRandomPointsInBounds(Model& InModel, const size_t& NumPoints, Array<Vector3D>& Points)
{
	Points.Reallocate(NumPoints);

	for (size_t i = 0; i < NumPoints; i++)
	{

		Vector3D point1;
		do
		{
			point1 = InModel.ModelTransform.GetRandomPointInBounds();
		} while (Points.Contains(point1) && IsPointTooClose(point1, Points));

		//TestSquare.push_back(DrawWireCube(point1, { 0.5, 0.5, 0.5 }, { 0.1f, 0.1f, 0.1f }, { 0.5, 0.5, 0.5 }));

		Points[i] = point1;
	}
	return Points;
}

void Voronoi::FractureDelaunayRandom(Model& InModel, const size_t& NumPoints)
{
	Array<Vector3D> points; 
	GenerateRandomPointsInBounds(InModel, NumPoints, points);

	GenerateVoronoiCellsDelaunay(points, InModel);
}

void Voronoi::DefinePlane(Vector3D& normal, const Vector3D& CurrentPoint, const Vector3D& closestPoint, Vector3D& Right, Vector3D& Up, Vector3D& PlaneCenter)
{
	normal = (CurrentPoint - closestPoint).Normalised();
	
	//if normal is nearly parallel to up vector, use right vector instead
	Vector3D arbitraryUp = Vector3D::Up;
	if (fabs(Vector3D::Dot(normal, arbitraryUp)) > 0.99f) {
		arbitraryUp = Vector3D(1, 0, 0);
	}

	Right = Vector3D::Cross(normal, arbitraryUp).Normalised();

	Up = Vector3D::Cross(Right, normal).Normalised();

	PlaneCenter = (CurrentPoint + closestPoint) / 2;
}

bool Voronoi::IsPointInPolygon(const Vector3D& Normal, const Array<Vector3D>& Polygon, const Vector3D& center)
{


	//Check if any point is on the wrong side of the plane, i.e. on the negative side of the normal
	for (size_t i = 0; i < Polygon.GetSize(); i++)
	{
		const float d = Vector3D::Dot(Polygon[i] - center, Normal);

		if (d < 0 && !MathCore::IsNearlyZero(d)) {
			return false;
		}
	}
	return true;
}

bool Voronoi::IsPointTooClose(const Vector3D& Point, const Array<Vector3D>& Points)
{
	for (const auto& p : Points)
	{
		double d = (Point - p).GetSquaredLength();
		if (d < 0.001) return true;

	}

	return false;
}

Vector3D Voronoi::GetCircumCenter(const Vector3D& A, const Vector3D& B, const Vector3D& C, const Vector3D& D)
{
	Vector3D a = A - D;
	Vector3D b = B - D;
	Vector3D c = C - D;

	double denom = 2.0 * Vector3D::Dot(a, Vector3D::Cross(b, c));

	Vector3D center =
		D +
		( Vector3D::Cross(b, c) * Vector3D::Dot(a, a) +
			Vector3D::Cross(c, a) * Vector3D::Dot(b, b) +
			Vector3D::Cross(a, b) * Vector3D::Dot(c, c)) / denom;
	return center;
}

void Voronoi::ClipCellToBox(const Model& InModel, Array<Face>& Cell)
{
	Array<Face> ClippingPlanes = InModel.BoundingBox->Faces;

	Array<Face> newCell;

	for (auto& plane : ClippingPlanes)
	{
		Vector3D Normal = Vector3D::Cross(plane.Vertices[1] - plane.Vertices[0], plane.Vertices[2] - plane.Vertices[0]).Normalised();

		if (Vector3D::Dot(Normal, plane.Vertices[0] - Vector3D::Zero) < 0) Normal = -Normal;

		const double d = -Vector3D::Dot(Normal, plane.Vertices[0]);

		Face IntersectFace;

		for (auto& face : Cell)
		{
			if (face.Vertices.IsEmpty())
			{
				continue;
			};


			Face NewFace;

			for (size_t i = 0; i < face.Vertices.GetSize(); i++)
			{
				Vector3D point = face.Vertices[i];
				Vector3D next = face.Vertices[(i + 1) % face.Vertices.GetSize()];
				const double D1 = Vector3D::Dot(Normal, point) + d;
				const double D2 = Vector3D::Dot(Normal, next) + d;

				if (D1 <= 0 && D2 <= 0)
				{
					NewFace.Vertices.Add(next);
				}
				else if (D1 > 0 && MathCore::IsNearlyZero(D2))
				{
					NewFace.Vertices.Add(next);
				}
				else if (MathCore::IsNearlyZero(D1) && D2 > 0)
				{
					continue;
				}
				else if (D1 <= 0 && D2 > 0)
				{
					Vector3D intersectPoint = Vector3D::GetLineIntersectionPointWithPlane(Normal, d, point, next);
					NewFace.Vertices.Add(intersectPoint);
					IntersectFace.Vertices.Add(intersectPoint);
					
				}
				else if (D1 > 0 && D2 <= 0)
				{
					Vector3D intersectPoint = Vector3D::GetLineIntersectionPointWithPlane(Normal, d, point, next);
					NewFace.Vertices.Add(intersectPoint);
					IntersectFace.Vertices.Add(intersectPoint);
					NewFace.Vertices.Add(next);
				}
			}
			face = NewFace;
		}
		Cell.Add(IntersectFace);
	}

	//for (auto& face : Cell)
	//{
	//	newCell.Add(ClipFaceToBox(ClippingPlanes, face, Vector3D::Zero));
	//}
	//Cell = newCell;
}

Face Voronoi::ClipFaceToBox(const Array<Face>& ClippingPlanes, const Face& ClippedFace, const Vector3D& Center)
{
	//Need to create faces from intersect points. 
	//Instead clip each cell face by bounds face, that way can get intersect face
	Face clippedFace = ClippedFace;

	for (const auto& face : ClippingPlanes)
	{
		if (clippedFace.Vertices.IsEmpty())
		{
			return clippedFace;
		};

		Vector3D Normal = Vector3D::Cross(face.Vertices[1] - face.Vertices[0], face.Vertices[2] - face.Vertices[0]).Normalised();

		if (Vector3D::Dot(Normal, face.Vertices[0] - Center) < 0) Normal = -Normal;

		const double d = -Vector3D::Dot(Normal, face.Vertices[0]);

		Face NewFace;

		for (size_t i = 0; i < clippedFace.Vertices.GetSize(); i++)
		{
			Vector3D point = clippedFace.Vertices[i];
			Vector3D next = clippedFace.Vertices[(i + 1) % clippedFace.Vertices.GetSize()];
			const double D1 = Vector3D::Dot(Normal, point) + d;
			const double D2 = Vector3D::Dot(Normal, next) + d;

			if (D1 <= 0 && D2 <= 0)
			{
				NewFace.Vertices.Add(next);
			}
			else if (D1 > 0 && MathCore::IsNearlyZero(D2))
			{
				NewFace.Vertices.Add(next);
			}
			else if (MathCore::IsNearlyZero(D1) && D2 > 0)
			{
				continue;
			}
			else if (D1 <= 0 && D2 > 0)
			{
				NewFace.Vertices.Add(Vector3D::GetLineIntersectionPointWithPlane(Normal, d, point, next));
			}
			else if (D1 > 0 && D2 <= 0)
			{
				NewFace.Vertices.Add(Vector3D::GetLineIntersectionPointWithPlane(Normal, d, point, next));
				NewFace.Vertices.Add(next);
			}
		}
		clippedFace = NewFace;
	}
	return clippedFace;
}

void Voronoi::GenerateVoronoiCellsDelaunay(Array<Vector3D>& Points, const Model& InModel)
{
	//Points = { {0.8, 0.4, 0.2}, {0.4, 0.8, 0.6}, {0.1, 0.1, 0.1} };
	//Instead of this, extended ghost points that get clipped?
	for (const auto& p : InModel.BoundingBox->Vertices)
	{
		Vector3D dir = p.Position.Normalised();

		Points.Add(p.Position + (dir * 4) );
	}



	Array<Point> dtPoints;

	for (const auto& p : Points)
	{
		dtPoints.Add({ p.X, p.Y, p.Z });
	}

	DelaunayTriangulation test;

	Array<Tetrahedron> alltets;
	test.Triangulate(Points, alltets);

	DT dt;
	dt.insert(dtPoints.begin(), dtPoints.end());

	for (const auto& point : Points) {

		Array<Tetrahedron> tets;

		for (const auto& tet : alltets)
		{
			if (tet.point1 == point || tet.point2 == point || tet.point3 == point || tet.point4 == point)
			{
				tets.Add(tet);
			}
		}

		//for (auto cell = dt.finite_cells_begin();
		//	cell != dt.finite_cells_end();
		//	++cell)
		//{
		//	Vector3D a = { cell->vertex(0)->point().x(), cell->vertex(0)->point().y(), cell->vertex(0)->point().z() };
		//	Vector3D b = { cell->vertex(1)->point().x(), cell->vertex(1)->point().y(), cell->vertex(1)->point().z() };
		//	Vector3D c = { cell->vertex(2)->point().x(), cell->vertex(2)->point().y(), cell->vertex(2)->point().z() };
		//	Vector3D d = { cell->vertex(3)->point().x(), cell->vertex(3)->point().y(), cell->vertex(3)->point().z() };


		//	if (a == point || b == point || c == point || d == point)
		//	{
		//		tets.Add(Tetrahedron(a, b, c, d));
		//	}

		//}

		Array<Edge> Edges;
		for (const auto& tet : tets)
		{
			for (size_t i = 0; i < 4; i++)
			{
				if (tet[i] == point) continue;
				Edge e(tet[i], point);

				if (Edges.Contains(e)) continue;

				Edges.Add(e);
			}
		}


		Array<Face> faces;
		bool bIsInfinite = false;
		for (const auto& edge : Edges)
		{

			Array<Tetrahedron> ring;

			for (const auto& tet : tets)
			{
				if (tet.ContainsPoint(edge.P1) && tet.ContainsPoint(edge.P2))
				{
					ring.Add(tet);
				}
			}
			;
			Array<Vector3D> circumcenters;

			for (const auto& tet : ring)
			{
				circumcenters.Add(GetCircumCenter(tet.point1, tet.point2, tet.point3, tet.point4));
			}

			Vector3D axis = (edge.P2 - edge.P1).Normalised();

			Vector3D center = Vector3D::Zero;

			for (const auto& c : circumcenters)
			{
				center += c;
			}
			center = center / circumcenters.GetSize();

			Vector3D arbitraryUp = Vector3D::Up;
			if (fabs(Vector3D::Dot(axis, arbitraryUp)) > 0.99f) {
				arbitraryUp = Vector3D(1, 0, 0);
			}

			Vector3D u = Vector3D::Cross(axis, arbitraryUp).Normalised();
			Vector3D t = Vector3D::Cross(axis, u);

			std::vector<AnglePointPair> OrderedPoints;

			for (const auto& c : circumcenters)
			{
				Vector3D d = c - center;
				double x = Vector3D::Dot(d, u);
				double y = Vector3D::Dot(d, t);
				double angle = std::atan2(y, x);

				OrderedPoints.push_back(AnglePointPair(c, angle));

			}
			std::sort(OrderedPoints.begin(), OrderedPoints.end());
			Face f;
			for (const auto& p : OrderedPoints)
			{
				f.Vertices.Add(p.point);
			}
			faces.Add(f);
		}


		ClipCellToBox(InModel, faces);

		auto color = Vector3D::RandomRange(Vector3D(30, 30, 30), Vector3D(255, 255, 255));



		FracturePiece3D frac = CreateObjectRaw<FracturePiece3D>(faces, point);
		frac.color = color;
		Fractures.Add({ frac });
	}

}


FracturePiece3D::FracturePiece3D(Array<Face> cell, Vector3D Point) : WorldObject()
{

	InputManager* inputManager = Camera::GetActiveCamera()->GetActiveInputManager();
	LeftArrow = std::make_unique<InputAction>(GLFW_KEY_LEFT, inputManager, Camera::GetActiveWindow());

	LeftArrow->Actions.BindMember(this, &FracturePiece3D::Seperate);

	RightArrow = std::make_unique<InputAction>(GLFW_KEY_RIGHT, inputManager, Camera::GetActiveWindow());

	RightArrow->Actions.BindMember(this, &FracturePiece3D::Converge);

	dir = (Point - Vector3D::Zero).Normalised();

	shader = Shader("ColorShape", "/Shaders/");

	Array<Vector3D> newVerts;

	//dir = Vector3D::RandomRange(Vector3D::Zero, Vector3D(100, 100, 100));

	//dir = dir.Normalised();


	for (const auto& face : cell)
	{
		for (size_t i = 1; i + 1 < face.Vertices.GetSize(); i++)
		{
			size_t index = 0;
			if (newVerts.Contains(face.Vertices[0], index))
			{
				Inds.Add(index);
			}
			else
			{
				Inds.Add(newVerts.GetSize());
				newVerts.Add(face.Vertices[0]);
			}

			if (newVerts.Contains(face.Vertices[i], index))
			{
				Inds.Add(index);
			}
			else
			{
				Inds.Add(newVerts.GetSize());
				newVerts.Add(face.Vertices[i]);
			}

			if (newVerts.Contains(face.Vertices[i + 1], index))
			{
				Inds.Add(index);
			}
			else
			{
				Inds.Add(newVerts.GetSize());
				newVerts.Add(face.Vertices[i+1]);
			}

		}
	}

	
	for (const Vector3D& vert : newVerts) {
		Verts.Add(vert.X);
		Verts.Add(vert.Y);
		Verts.Add(vert.Z);
	}

	////for (size_t i = 1; i + 1< cell.GetSize(); i++)
	////{
	////	Inds.Add(0);
	////	Inds.Add(i);
	////	Inds.Add(i + 1);
	////}


	if (Inds.IsEmpty())
	{
		return;
	}

	::DataBuffers::GenBuffer(VAO);


	DataBuffers::BindVertexInfo(VAO, 0, 0, sizeof(Vector3D), 0, Vector3);


	::DataBuffers::BufferData(VAO, Verts.GetSize() * sizeof(float), Verts.GetFirstPtr(), BufferTargets::VERTEX);
	DataBuffers::BufferDataIndex(VAO, Inds.GetSize() * sizeof(uint16_t), Inds.GetFirstPtr());



	::Renderer::AddFracture(this);
}

void FracturePiece3D::Draw()
{
	shader.Use();

	DataBuffers::BindBuffer(VAO);

	DataBuffers::DrawVertexData(VAO);

	PerInstanceTransforms ubo;
	ubo.Model = transform.GetModelMatrix();
	ubo.Color = color;

	GlobalTransforms g;
	g.View = Camera::GetActiveCamera()->GetLook();

	g.Projection = Camera::GetActiveCamera()->GetProjection();

	shader.SetUniformBuffer(0, &g, sizeof(GlobalTransforms));

	shader.SetUniformBuffer(1, &ubo, sizeof(PerInstanceTransforms));

	::Renderer::Draw(Inds.GetSize());

}

void FracturePiece3D::Start()
{
	WorldObject::Start();
}

void FracturePiece3D::Tick(const double& DeltaTime)
{
	WorldObject::Tick(DeltaTime);

	//if (glfwGetTime() > 10)
	//{

	//	transform.Position = transform.Position + (dir * 5) * DeltaTime;
	//}

}

void FracturePiece3D::Seperate()
{

	transform.Position += (dir * 5) * World->GetDeltaTime();

}
void FracturePiece3D::Converge()
{
	transform.Position -= (dir * 5) * World->GetDeltaTime();
}


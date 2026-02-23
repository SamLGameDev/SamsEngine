
#include "Voronoi.h"
#include "transform.h"
#include <algorithm>
#include <iostream>
#include <thread>

#include "Camera.h"
#include "DataBuffers.h"
#include "DelaunayTriangulation.h"
#include "FileSaving.h"
#include "HardwareDetails.h"
#include "MathCore.h"
#include "ObjectFactory.h"
#include "InterfaceRenderer.h"
#include "PlaneClipping.h"
#include "glm/gtc/type_ptr.hpp"
#include "PerformanceRecord.h"

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

	Vector3D center = intersectFace.GetCenter();

	//As plane is box, we can assume [1] and [2] from zero would cove x and y
	Vector3D normal = Vector3D::Cross(intersectFace.Vertices[1] - intersectFace.Vertices[0], intersectFace.Vertices[2] - intersectFace.Vertices[0]).Normalised();

	//If its not facing outwards
	if (Vector3D::Dot(normal, intersectFace.Vertices[0] - Vector3D::Zero) < 0) normal = -normal;

	Vector3D::OrderByAngle(intersectFace.Vertices, center, normal);

	if (intersectFace.Vertices.GetSize() >= 3)	newFaces.Add(intersectFace);

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

void Voronoi::FracturePlaneRandom(Model& InModel, const size_t& NumPoints, const size_t& PointSetIndex)
{
	Array<Vector3D> points;

	DataRecord record;
	record.CPU = UHardwareDetails::GetCPU();
	record.API = UHardwareDetails::API;
	record.Card = UHardwareDetails::GetGPU();
	record.OS = UHardwareDetails::GetOS();
	record.RAM = UHardwareDetails::GetRAM_GB();

	const Vector2D range = { PointSetIndex * 10, ((PointSetIndex + 1) * 10) - 1 };

	if (NumPoints == 10)
	{
		UFileWriter::Load("/ExperimentData/SetOfTen.txt", points, range);
	}
	else if ( NumPoints == 100)
	{
		UFileWriter::Load("/ExperimentData/SetOfHundred.txt", points, range);
	}
	else
	{
		UFileWriter::Load("/ExperimentData/SetOfThousand.txt", points, range);
	}

	const double TimeBeforeComputation = glfwGetTime();

	Fractures.ReSize(points.GetSize());
	for (size_t i = 0; i < points.GetSize(); i++)
	{
		Vector3D currentPoint = points[i];

		Array<Face> Faces = InModel.BoundingBox->Faces;

		Vector3D normal, right, up, center;

		for (size_t j = 0; j < points.GetSize(); j++)
		{
			if (i == j)continue;
			DefinePlane(normal, currentPoint, points[j], right, up, center);
			PlaneClipping::ClipCellByFace(Faces, center, normal);

			//SliceShapeByPlane(points, i, currentPoint, Faces, normal, right, up, center, j);
		}

		//auto color = Vector3D::RandomRange(Vector3D(30, 30, 30), Vector3D(255, 255, 255));

		FracturePiece3D frac = CreateObjectRaw<FracturePiece3D>(Faces, currentPoint);
		//frac.color = color;
		Fractures.Add({ frac });

	}

	const double TimeTaken = glfwGetTime() - TimeBeforeComputation;
	
	if (NumPoints == 10)
	{
		record.TenPoints = std::to_string(TimeTaken);
	}
	else if (NumPoints == 100)
	{
		record.OneHundredPoints = std::to_string(TimeTaken);
	}
	else
	{
		record.OneThousandPoints = std::to_string(TimeTaken);
	}

	DataRecorder::SaveDataRecord(record, "/ExperimentData/CellGenerationResults.json");

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

		Points[i] = point1;
	}
	return Points;
}

void Voronoi::FractureDelaunayRandom(Model& InModel, const size_t& NumPoints)
{
	Array<Vector3D> points; 
	GenerateRandomPointsInBounds(InModel, NumPoints, points);

	GenerateVoronoiCellsDelaunay(points, InModel);

	InputManager* inputManager = Camera::GetActiveCamera()->GetActiveInputManager();
	Next = std::make_unique<InputAction>(GLFW_KEY_I, inputManager, Camera::GetActiveWindow());
	Next->Actions.BindMember(this, &Voronoi::NextCell);

	Fractures[0].ToggleRendering();

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
	//make D the origin
	Vector3D a = A - D;
	Vector3D b = B - D;
	Vector3D c = C - D;

	const double denom = 2.0 * Vector3D::Dot(a, Vector3D::Cross(b, c));

	//offset D by each point

	Vector3D center =
		D +
		( Vector3D::Cross(b, c) * a.GetSquaredLength() +
			Vector3D::Cross(c, a) * b.GetSquaredLength() +
			Vector3D::Cross(a, b) * c.GetSquaredLength()) / denom;

	return center;
}

void Voronoi::ClipVertexToPlane(const Vector3D& Normal, const double& D, Face& IntersectFace, const Vector3D& Vertex,
                                const Vector3D& NextVertex, Face& NewFace)
{
	const double d1 = Vector3D::Dot(Normal, Vertex) + D;
	const double d2 = Vector3D::Dot(Normal, NextVertex) + D;

	const auto inside = [&](const double& d) { return d <= 0; };
	const auto outside = [&](const double& d) { return d > 0; };
	const auto onPlane = [&](const double& d) { return MathCore::IsNearlyZero(d); };

	if (inside(d1) && inside(d2)|| (outside(d1) && onPlane(d2))) NewFace.Vertices.Add(NextVertex);

	else if (MathCore::IsNearlyZero(d1) && d2 > 0) return;

	else if (inside(d1) && outside(d2))
	{
		const Vector3D intersectPoint = Vector3D::GetLineIntersectionPointWithPlane(Normal, D, Vertex, NextVertex);
		NewFace.Vertices.Add(intersectPoint);
					
		IntersectFace.Vertices.Add(intersectPoint);
	}

	else if (outside(d1) && inside(d2))
	{
		const Vector3D intersectPoint = Vector3D::GetLineIntersectionPointWithPlane(Normal, D, Vertex, NextVertex);

		NewFace.Vertices.Add(intersectPoint);
		IntersectFace.Vertices.Add(intersectPoint);
		NewFace.Vertices.Add(NextVertex);
	}
}

void Voronoi::GetFaceAxis(const Vector3D& Normal, Vector3D& T, Vector3D& U)
{
	Vector3D arbitraryUp = Vector3D::Up;
	if (fabs(Vector3D::Dot(Normal, arbitraryUp)) > 0.99f) {
		arbitraryUp = Vector3D(1, 0, 0);
	}

	U = Vector3D::Cross(Normal, arbitraryUp).Normalised();
	T = Vector3D::Cross(Normal, U);
}

void Voronoi::ClipCellToPlane(Array<Face>& Cell, const Face& Plane)
{
	//As plane is box, we can assume [1] and [2] from zero would cove x and y
	Vector3D normal = Vector3D::Cross(Plane.Vertices[1] - Plane.Vertices[0], Plane.Vertices[2] - Plane.Vertices[0]).Normalised();

	//If its not facing outwards
	if (Vector3D::Dot(normal, Plane.Vertices[0] - Vector3D::Zero) < 0) normal = -normal;

	const double d = -Vector3D::Dot(normal, Plane.Vertices[0]);

	Face intersectFace;

	for (auto& face : Cell)
	{
		if (face.Vertices.IsEmpty())
		{
			continue;
		}

		Face newFace;

		for (size_t i = 0; i < face.Vertices.GetSize(); i++)
		{
			const Vector3D& point = face.Vertices[i];
			const Vector3D& next = face.Vertices[(i + 1) % face.Vertices.GetSize()];
			ClipVertexToPlane(normal, d, intersectFace, point, next, newFace);
		}
		face = newFace;
	}

	if (intersectFace.Vertices.GetSize() < 3) return;

	Vector3D center = intersectFace.GetCenter();

	Vector3D::OrderByAngle(intersectFace.Vertices, center, normal);

	Cell.Add(intersectFace);
}

void Voronoi::ClipCellToBox(const Model& InModel, Array<Face>& Cell)
{
	Array<Face> ClippingPlanes = InModel.BoundingBox->Faces;

	Array<Face> newCell;

	for (const auto& plane : ClippingPlanes)
	{
		ClipCellToPlane(Cell, plane);
	}

}

void Voronoi::GetAllIncidentTets(const Array<Tetrahedron>& Tetrahedra, const Vector3D& Point, Array<TetRing>& Rings)
{
	for (const auto& tet : Tetrahedra)
	{
		if (tet.point1 == Point || tet.point2 == Point || tet.point3 == Point || tet.point4 == Point)
		{
			for (size_t i = 0; i < 4; i++)
			{
				if (tet[i] == Point) continue;

				const TetRing ring({ tet[i], Point }, { tet });

				size_t index = 0;
				if (Rings.Contains(ring, index))
				{
					Rings[index].Tets.Add(tet);
				}
				Rings.Add(ring);
			}
		}
	}
}

void Voronoi::GetCellFace(Array<Face>& Faces, const TetRing& Ring)
{
	Array<Vector3D> circumcenters;
	Vector3D center = Vector3D::Zero;


	for (const auto& tet : Ring.Tets)
	{
		circumcenters.Add(GetCircumCenter(tet.point1, tet.point2, tet.point3, tet.point4));

		center += *circumcenters.GetLastPtr();
	}

	center = center / circumcenters.GetSize();

	
	const Vector3D normal = (Ring.edge.P2 - Ring.edge.P1).Normalised();
	
	Vector3D::OrderByAngle(circumcenters, center, normal);

	Faces.Add({ circumcenters });

}

Array<Face> Voronoi::GetCell(const Array<Tetrahedron>& tetrahedra, const Vector3D& point)
{
	Array<TetRing> rings;

	GetAllIncidentTets(tetrahedra, point, rings);

	Array<Face> faces;
	for (const auto& ring : rings)
	{
		GetCellFace(faces, ring);
	}
	return faces;
}

void Voronoi::GenerateVoronoiCellDelaunay(const Model& InModel, const Array<Tetrahedron>& Tetrahedra, const Vector3D& Point)
{
	Array<Face> faces = GetCell(Tetrahedra, Point);
	ClipCellToBox(InModel, faces);

	auto color = Vector3D::RandomRange(Vector3D(30, 30, 30), Vector3D(255, 255, 255));
	FracturePiece3D frac = CreateObjectRaw<FracturePiece3D>(faces, Point);
	frac.color = color;
	Fractures.Emplace( std::move(frac) );
}

void Voronoi::NextCell()
{
	Fractures[current].bIsHidden = true;

	current = (current + 1) % Fractures.GetSize();

	Fractures[current].bIsHidden = false;

}

void Voronoi::GenerateVoronoiCellsDelaunay(const Array<Vector3D>& Points, const Model& InModel)
{
	
	DelaunayTriangulation dt;

	dt.Triangulate(Points);
	for (const auto& point : Points)
	{
		GenerateVoronoiCellDelaunay(InModel, dt.Tetrahedrons, point);
	}

}

void Voronoi::GenerateNewPointSets(Model& InModel)
{
	Array<Vector3D> TenPoints;
	TenPoints.ReSize(10 * 145);
	Array<Vector3D> HundredPoints;
	TenPoints.ReSize(100 * 145);
	Array<Vector3D> ThousandPoints;
	TenPoints.ReSize(1000 * 145);

	for (size_t i = 0; i < 145; i++)
	{
		Array<Vector3D> points;
		GenerateRandomPointsInBounds(InModel, 10, points);
		TenPoints.Emplace(std::move(points));

		GenerateRandomPointsInBounds(InModel, 100, points);
		HundredPoints.Emplace(std::move(points));

		GenerateRandomPointsInBounds(InModel, 1000, points);
		ThousandPoints.Emplace(std::move(points));

	}

	UFileWriter::SaveArray("/ExperimentData/SetOfTen.txt", TenPoints);
	UFileWriter::SaveArray("/ExperimentData/SetOfHundred.txt", HundredPoints);
	UFileWriter::SaveArray("/ExperimentData/SetOfThousand.txt", ThousandPoints);
}


FracturePieceGPU::FracturePieceGPU(const GLuint& InVoronoiOut)
{
	shader = Shader("VoronoiTest", "/Shaders/");
	::Renderer::AddFracture(this);
	VoronoiOut = InVoronoiOut;
}

void FracturePieceGPU::Draw()
{
	shader.Use();

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, VoronoiOut);
	GlobalTransforms g;
	g.View = Camera::GetActiveCamera()->GetLook();

	g.Projection = Camera::GetActiveCamera()->GetProjection();

	shader.SetUniformBuffer(0, &g, sizeof(GlobalTransforms));

	shader.SetUniformBuffer(2, InstanceInfo, sizeof(VoronoiCellInstanceInfo));

	glDrawArraysInstanced(GL_POINTS, 0, 1, 10);
}

FracturePiece3D::~FracturePiece3D()
{
	::Renderer::RemoveFracture(this);
}

void FracturePiece3D::AddOrMakeInd(const Vector3D& Vert)
{
	size_t index = 0;
	if (Verts.Contains(Vert, index))
	{
		Inds.Add(index);
	}
	else
	{
		Inds.Add(Verts.GetSize());
		Verts.Add(Vert);
	}
}

void FracturePiece3D::TriangulateCell(const Array<Face>& cell)
{
	for (const auto& face : cell)
	{
		for (size_t i = 1; i + 1 < face.Vertices.GetSize(); i++)
		{
			AddOrMakeInd(face.Vertices[0]);

			AddOrMakeInd(face.Vertices[i]);

			AddOrMakeInd(face.Vertices[i+1]);

		}
	}
}


FracturePiece3D::FracturePiece3D(const Array<Face>& cell, const Vector3D& CellPoint)
{
	//SetupControls(Point);

	for (const auto& face : cell)
	{
		if (face.Vertices.GetSize() < 3)
		{
			continue;
		}
		Face newFace;
		for (const auto& vert : face.Vertices)
		{
			if (newFace.Vertices.Contains(vert))
			{
				continue;
			}
			newFace.Vertices.Add(vert);
		}

		if (newFace.Vertices.GetSize() < 3)
		{
			continue;
		}

		CellFaces.Add(newFace);

		Vector3D::OrderByAngle(CellFaces.GetLastPtr()->Vertices, newFace.GetCenter(), Vector3D::GetPlaneNormal(CellFaces.GetLastPtr()->Vertices, newFace.GetCenter()));

	}

	//shader = Shader("ColorShape", "/Shaders/");

	TriangulateCell(cell);

	if (Inds.IsEmpty())
	{
		return;
	}
	this->Point = CellPoint;

	//BufferData();
}

void FracturePiece3D::SetupControls(const Vector3D& point)
{
	InputManager* inputManager = Camera::GetActiveCamera()->GetActiveInputManager();
	LeftArrow = std::make_unique<InputAction>(GLFW_KEY_LEFT, inputManager, Camera::GetActiveWindow());

	LeftArrow->Actions.BindMember(this, &FracturePiece3D::Seperate);

	RightArrow = std::make_unique<InputAction>(GLFW_KEY_RIGHT, inputManager, Camera::GetActiveWindow());

	RightArrow->Actions.BindMember(this, &FracturePiece3D::Converge);


	Hide = std::make_unique<InputAction>(GLFW_KEY_H, inputManager, Camera::GetActiveWindow());

	Hide->Actions.BindMember(this, &FracturePiece3D::ToggleRendering);

	dir = (point - Vector3D::Zero).Normalised();


}

void FracturePiece3D::BufferData()
{
	::DataBuffers::GenBuffer(VAO);

	DataBuffers::BindVertexInfo(VAO, 0, 0, sizeof(Vector3D), 0, Vector3);

	::DataBuffers::BufferData(VAO, Verts.GetSize() * sizeof(Vector3D), Verts.GetFirstPtr(), BufferTargets::VERTEX);
	DataBuffers::BufferDataIndex(VAO, Inds.GetSize() * sizeof(uint16_t), Inds.GetFirstPtr());

	::Renderer::AddFracture(this);
}

void FracturePiece3D::Draw()
{
	if (bIsHidden) return;
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

}

void FracturePiece3D::Seperate()
{

	transform.Position += (dir * 5) * World->GetDeltaTime();

}
void FracturePiece3D::Converge()
{
	transform.Position -= (dir * 5) * World->GetDeltaTime();
}

void FracturePiece3D::ToggleRendering()
{
	bIsHidden = !bIsHidden;
}


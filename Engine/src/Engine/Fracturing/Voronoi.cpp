
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
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_3.h>
#include <CGAL/IO/polygon_mesh_io.h>
#include <CGAL/Surface_mesh.h>
#include <CGAL/make_conforming_constrained_Delaunay_triangulation_3.h>
#include <CGAL/IO/write_MEDIT.h>

#include "ComputeShader/UComputeShader.h"

using Kernel = CGAL::Exact_predicates_inexact_constructions_kernel;
using K = CGAL::Exact_predicates_inexact_constructions_kernel;
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

	const Vector2D range = { PointSetIndex * 10, ((PointSetIndex + 1) * 10) };

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

void Voronoi::CreateMeshFractureGPU(VoronoiSSBOIn* buffer, Array<Vector3D> points, InTets tets, GLuint VoronoiIn, GLuint VoronoiOut, GLuint ClippedOutInd, GLuint InTetsInd)
{
	UComputeShader voronoiCompute = UComputeShader("VoronoiCellGeneration", "/Shaders/Voronoi/");

	voronoiCompute.Use();

	::DataBuffers::GenerateShaderStorageBuffer(VoronoiIn, sizeof(VoronoiSSBOIn), 1);
	::DataBuffers::GenerateShaderStorageBuffer(VoronoiOut, sizeof(VOut), 3);

	void* inPtr = ::DataBuffers::MapBufferMemory(VoronoiIn, sizeof(VoronoiSSBOIn));

	std::cout << "buffer size: " << sizeof(VoronoiSSBOIn) << std::endl;
	std::cout << "buffer size: " << sizeof(VOutLarge) << std::endl;

	GLenum error = glGetError();
	if (error != GL_NO_ERROR)
	{
		std::cerr << "OpenGL error before buffer generation: " << error << std::endl;
	}


	VoronoiSSBOIn* inData = static_cast<VoronoiSSBOIn*>(inPtr);

	memcpy(inData, buffer, sizeof(VoronoiSSBOIn));


	error = glGetError();
	if (error != GL_NO_ERROR)
	{
		std::cerr << "OpenGL error before buffer generation: " << error << std::endl;
	}
	::DataBuffers::UnMapBufferMemory(VoronoiIn);


	error = glGetError();
	if (error != GL_NO_ERROR)
	{
		std::cerr << "OpenGL error before buffer generation: " << error << std::endl;
	}

	voronoiCompute.Dispatch(points.GetSize(), 1, 1);


	error = glGetError();
	if (error != GL_NO_ERROR)
	{
		std::cerr << "OpenGL error before buffer generation: " << error << std::endl;
	}
	voronoiCompute.WaitForCompletion();


	error = glGetError();
	if (error != GL_NO_ERROR)
	{
		std::cerr << "OpenGL error before buffer generation: " << error << std::endl;
	}

	::DataBuffers::RemoveBuffer(VoronoiIn);


	error = glGetError();
	if (error != GL_NO_ERROR)
	{
		std::cerr << "OpenGL error before buffer generation: " << error << std::endl;
	}

	UComputeShader clippingCompute = UComputeShader("VoronoiClipping", "/Shaders/Voronoi/");

	error = glGetError();
	if (error != GL_NO_ERROR)
	{
		std::cerr << "OpenGL error before buffer generation: " << error << std::endl;
	}


	clippingCompute.Use();
	::DataBuffers::BindShaderStorageBuffer(VoronoiOut, 3, sizeof(VOut));

	::DataBuffers::GenerateShaderStorageBuffer(InTetsInd, sizeof(InTets), 4);
	::DataBuffers::GenerateShaderStorageBuffer(ClippedOutInd, sizeof(VOutLarge), 5);

	void* inTetPtr = ::DataBuffers::MapBufferMemory(InTetsInd, sizeof(InTets));

	InTets* inTetsData = static_cast<InTets*>(inTetPtr);

	memcpy(inTetsData, &tets, sizeof(InTets));

	error = glGetError();
	if (error != GL_NO_ERROR)
	{
		std::cerr << "OpenGL error before buffer generation: " << error << std::endl;
	}


	::DataBuffers::UnMapBufferMemory(InTetsInd);

	clippingCompute.Dispatch(points.GetSize(), 1, 1);
	clippingCompute.WaitForCompletion();

	void* outPtr = ::DataBuffers::MapBufferMemory(ClippedOutInd, sizeof(VOutLarge));

	VOutLarge* clippedOutData = static_cast<VOutLarge*>(outPtr);

	 error = glGetError();
	if (error != GL_NO_ERROR)
	{
		std::cerr << "OpenGL error before buffer generation: " << error << std::endl;
	}


	for (size_t i = 0; i < points.GetSize(); i++)
	{
		if (clippedOutData->CutCells[i].NumInds == 0) continue;
		FracturePieceGPU frac = CreateObjectRaw<FracturePieceGPU>(clippedOutData->CutCells[i], points[i]);
		GPUFractures.Add({ frac });
	}


	::DataBuffers::RemoveBuffer(VoronoiOut);
	::DataBuffers::RemoveBuffer(InTetsInd);
	::DataBuffers::RemoveBuffer(ClippedOutInd);
}

void Voronoi::FracturePlaneRandomGPU(Model& InModel, const size_t& NumPoints, const size_t& PointSetIndex)
{

	InTets tets;
	tets.NumTets = 0;

	for (const auto& subMesh : InModel.Meshes) {

		//CGAL::Surface_mesh<K::Point_3> dtPoints;
		std::vector<K::Point_3> dtPoints;
		dtPoints.reserve(subMesh.Vertices.GetSize());

		//if (!CGAL::IO::read_polygon_mesh("D:/Comp303-SL295211-VoronoiClipping/Engine/Contents/Models/Bunny/Bunny.obj", dtPoints)) {
		//	std::cerr << "Error: cannot read file "  << std::endl;
		//}

		std::vector<std::vector<uint16_t>> inds;
		inds.reserve(subMesh.Indices.GetSize() / 3);

		for (size_t i = 0; i + 2 < subMesh.Indices.GetSize(); i += 3)
		{
			std::vector<uint16_t> tri = { subMesh.Indices[i], subMesh.Indices[i + 1], subMesh.Indices[i + 2] };

			inds.push_back(tri);
		}

		for (const auto& p : subMesh.Vertices)
		{
			dtPoints.push_back({ p.Position.X, p.Position.Y, p.Position.Z });
		}

		auto dt = CGAL::make_conforming_constrained_Delaunay_triangulation_3(dtPoints, inds);

		for (auto cell = dt.triangulation().finite_cells_begin(); cell != dt.triangulation().finite_cells_end(); ++cell)
		{
			const Point& p0 = cell->vertex(0)->point();
			const Point& p1 = cell->vertex(1)->point();
			const Point& p2 = cell->vertex(2)->point();
			const Point& p3 = cell->vertex(3)->point();

			const Vector3D v0 = Vector3D(p0.x(), p0.y(), p0.z());
			const Vector3D v1 = Vector3D(p1.x(), p1.y(), p1.z());
			const Vector3D v2 = Vector3D(p2.x(), p2.y(), p2.z());
			const Vector3D v3 = Vector3D(p3.x(), p3.y(), p3.z());

			tets.Tets[tets.NumTets].TetFaces[0] = { v0, v1, v2 };
			tets.Tets[tets.NumTets].TetFaces[1] = { v0, v1, v3 };
			tets.Tets[tets.NumTets].TetFaces[2] = { v0, v2, v3 };
			tets.Tets[tets.NumTets].TetFaces[3] = { v1, v2, v3 };
			tets.NumTets++;

		}
	}

	VoronoiSSBOIn* buffer = new VoronoiSSBOIn;
	VOut* vOut = new VOut;
	VOutLarge* ClippedOut = new VOutLarge;
	vOut->NumCells = 0;
	vOut->DebugNum = 10;
	ClippedOut->NumCells = 0;
	ClippedOut->DebugNum = 10;

	for (size_t i = 0; i < InModel.BoundingBox->Faces.GetSize(); i++)
	{
		Face& face = InModel.BoundingBox->Faces[i];
		for (size_t j = 0; j < face.Vertices.GetSize(); j++)
		{
			buffer->BoundingBoxFaces[i].Verts[j] = face.Vertices[j];
		}
		buffer->BoundingBoxFaces[i].NumVerts = face.Vertices.GetSize();
	}

	std::string DataToLoad = "/ExperimentData/SetOfTen.txt";

	GLuint VoronoiIn, VoronoiOut, ClippedOutInd, InTetsInd;

	::DataBuffers::GenBuffer(VoronoiIn);
	::DataBuffers::GenBuffer(VoronoiOut);
	::DataBuffers::GenBuffer(ClippedOutInd);
	::DataBuffers::GenBuffer(InTetsInd);

	size_t numPoints = 10;

	std::cout << sizeof(InTets) << std::endl;

	for (size_t i = 0; i < 1; i++)
	{
		Array<Vector3D> points;

		const Vector2D range = { i * numPoints, ((i + 1) * numPoints) };

		UFileWriter::Load(DataToLoad, points, range);

		for (size_t i = 0; i < points.GetSize(); i++)
		{
			buffer->Points[i] = points[i];
		}
		buffer->NumPoints = points.GetSize();

		CreateMeshFractureGPU(buffer, points, tets, VoronoiIn, VoronoiOut, ClippedOutInd, InTetsInd);
	}


	//DataToLoad = "/ExperimentData/SetOfHundred.txt";

	//numPoints = 100;

	//for (size_t i = 0; i < 145; i++)
	//{
	//	Array<Vector3D> points;

	//	const Vector2D range = { i * numPoints, ((i + 1) * numPoints) };

	//	UFileWriter::Load(DataToLoad, points, range);

	//	for (size_t i = 0; i < points.GetSize(); i++)
	//	{
	//		buffer->Points[i] = points[i];
	//	}
	//	buffer->NumPoints = points.GetSize();

	//	CreateMeshFractureGPU(buffer, points, tets, VoronoiIn, VoronoiOut, ClippedOutInd, InTetsInd);
	//}

	//DataToLoad = "/ExperimentData/SetOfThousand.txt";

	//numPoints = 1000;

	//for (size_t i = 0; i < 145; i++)
	//{
	//	Array<Vector3D> points;

	//	const Vector2D range = { i * numPoints, ((i + 1) * numPoints) };

	//	UFileWriter::Load(DataToLoad, points, range);

	//	for (size_t i = 0; i < points.GetSize(); i++)
	//	{
	//		buffer->Points[i] = points[i];
	//	}
	//	buffer->NumPoints = points.GetSize();

	//	CreateMeshFractureGPU(buffer, points, tets, VoronoiIn, VoronoiOut, ClippedOutInd, InTetsInd);
	//}

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
	normal = (closestPoint - CurrentPoint).Normalised();
	
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
void FracturePieceGPU::AddOrMakeInd(const Vector4D& Vert)
{
	//size_t index = 0;
	//if (Verts.Contains(Vert, index))
	//{
	//	Inds.Add(index);
	//}
	//else
	//{
	//	Inds.Add(Verts.GetSize());
	//	Verts.Add(Vert);
	//}
}

void FracturePieceGPU::TriangulateCell(const Cell cell)
{
	for (size_t j = 0; j < cell.NumFaces; j++)
	{
		for (size_t i = 1; i + 1 < cell.Faces[j].NumVerts; i++)
		{
			AddOrMakeInd(cell.Faces[j].Verts[0]);

			AddOrMakeInd(cell.Faces[j].Verts[i]);

			AddOrMakeInd(cell.Faces[j].Verts[i + 1]);

		}
	}
}
void FracturePieceGPU::AddOrMakeInd(const Vector3D& Vert)
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

void FracturePieceGPU::TriangulateCell(const Array<Face>& cell)
{
	for (const auto& face : cell)
	{
		for (size_t i = 1; i + 1 < face.Vertices.GetSize(); i++)
		{
			AddOrMakeInd(face.Vertices[0]);

			AddOrMakeInd(face.Vertices[i]);

			AddOrMakeInd(face.Vertices[i + 1]);

		}
	}
}


FracturePieceGPU::FracturePieceGPU(LargeCell& InVoronoiOut, const Vector3D& InPoint)
{

	for (size_t j = 0; j <  InVoronoiOut.NumFaces; j++)
	{
		Facew* face = &InVoronoiOut.Faces[j];
		Face newFace;
		for (size_t i = 0; i < face->NumVerts; i++)
		{
			Vector3D vert = Vector3D(face->Verts[i].X, face->Verts[i].Y, face->Verts[i].Z);
			newFace.Vertices.Add(vert);
		}
		CellFaces.Add(newFace);
	}

	TriangulateCell(CellFaces);

	SetupControls(InPoint);

	::DataBuffers::GenBuffer(VAO);

	DataBuffers::BindVertexInfo(VAO, 0, 0, sizeof(Vector3D), 0, Vector3);

	::DataBuffers::BufferData(VAO, Verts.GetSize() * sizeof(Vector3D), Verts.GetFirstPtr(), BufferTargets::VERTEX);
	DataBuffers::BufferDataIndex(VAO, Inds.GetSize() * sizeof(uint16_t), Inds.GetFirstPtr());

	::Renderer::AddFracture(this);

	shader = Shader("ColorShape", "/Shaders/");

	color = Vector3D::RandomRange(Vector3D(30, 30, 30), Vector3D(255, 255, 255));
	NumInds = Inds.GetSize();
}

FracturePieceGPU::FracturePieceGPU(Cell& InVoronoiOut, const Vector3D& InPoint)
{
	for (size_t j = 0; j < InVoronoiOut.NumFaces; j++)
	{
		Facew* face = &InVoronoiOut.Faces[j];
		Face newFace;
		for (size_t i = 0; i < face->NumVerts; i++)
		{
			Vector3D vert = Vector3D(face->Verts[i].X, face->Verts[i].Y, face->Verts[i].Z);
			newFace.Vertices.Add(vert);
		}
		CellFaces.Add(newFace);
	}

	TriangulateCell(CellFaces);

	SetupControls(InPoint);

	::DataBuffers::GenBuffer(VAO);

	DataBuffers::BindVertexInfo(VAO, 0, 0, sizeof(Vector3D), 0, Vector3);

	::DataBuffers::BufferData(VAO, Verts.GetSize() * sizeof(Vector3D), Verts.GetFirstPtr(), BufferTargets::VERTEX);
	DataBuffers::BufferDataIndex(VAO, Inds.GetSize() * sizeof(uint16_t), Inds.GetFirstPtr());

	::Renderer::AddFracture(this);

	shader = Shader("ColorShape", "/Shaders/");

	color = Vector3D::RandomRange(Vector3D(30, 30, 30), Vector3D(255, 255, 255));

	NumInds = Inds.GetSize();
}

FracturePieceGPU::FracturePieceGPU(RawCell& InVoronoiOut, const Vector3D& InPoint)
{
	Verts.ReSize(InVoronoiOut.NumVerts);

	for (size_t i = 0; i < InVoronoiOut.NumVerts; i++)
	{
		Verts.Add(Vector3D(InVoronoiOut.Verts[i].X, InVoronoiOut.Verts[i].Y, InVoronoiOut.Verts[i].Z));
	}

	for (size_t i = 0; i < InVoronoiOut.NumInds; i++)
	{
		Inds.Add(static_cast<uint16_t>(InVoronoiOut.Inds[i]));
	}

	if (Inds.GetSize() < 3)
	{
		return;
	}

	SetupControls(InPoint);

	::DataBuffers::GenBuffer(VAO);

	DataBuffers::BindVertexInfo(VAO, 0, 0, sizeof(Vector3D), 0, Vector3);

	::DataBuffers::BufferData(VAO, Verts.GetSize() * sizeof(Vector3D), Verts.GetFirstPtr(), BufferTargets::VERTEX);
	DataBuffers::BufferDataIndex(VAO, Inds.GetSize() * sizeof(uint16_t), Inds.GetFirstPtr());

	::Renderer::AddFracture(this);

	shader = Shader("ColorShape", "/Shaders/");

	color = Vector3D::RandomRange(Vector3D(30, 30, 30), Vector3D(255, 255, 255));
	NumInds = Inds.GetSize();
}

FracturePieceGPU::~FracturePieceGPU()
{
	::Renderer::RemoveFracture(this);
}

void FracturePieceGPU::Draw()
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

	::Renderer::Draw(NumInds);
}

void FracturePieceGPU::SetupControls(const Vector3D& point)
{
	InputManager* inputManager = Camera::GetActiveCamera()->GetActiveInputManager();
	LeftArrow = std::make_unique<InputAction>(GLFW_KEY_LEFT, inputManager, Camera::GetActiveWindow());

	LeftArrow->Actions.BindMember(this, &FracturePieceGPU::Seperate);

	RightArrow = std::make_unique<InputAction>(GLFW_KEY_RIGHT, inputManager, Camera::GetActiveWindow());

	RightArrow->Actions.BindMember(this, &FracturePieceGPU::Converge);


	//Hide = std::make_unique<InputAction>(GLFW_KEY_H, inputManager, Camera::GetActiveWindow());

	//Hide->Actions.BindMember(this, &FracturePiece3D::ToggleRendering);

	dir = (point - Vector3D::Zero).Normalised();


}

void FracturePieceGPU::Start()
{
	WorldObject::Start();
}

void FracturePieceGPU::Tick(const double& DeltaTime)
{
	WorldObject::Tick(DeltaTime);
}

void FracturePieceGPU::Seperate()
{

	transform.Position += (dir * 5) * World->GetDeltaTime();
}

void FracturePieceGPU::Converge()
{
	transform.Position -= (dir * 5) * World->GetDeltaTime();
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

	transform.Position = { -7, 0, 0 };
	SetupControls(CellPoint);

	for (const auto& face : cell)
	{
		CellFaces.Add(face);

		//Vector3D::OrderByAngle(CellFaces.GetLastPtr()->Vertices, newFace.GetCenter(), Vector3D::GetPlaneNormal(CellFaces.GetLastPtr()->Vertices, newFace.GetCenter()));
	}

	shader = Shader("ColorShape", "/Shaders/");

	TriangulateCell(cell);

	if (Inds.IsEmpty())
	{
		return;
	}
	this->Point = CellPoint;

	BufferData();
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

	color = Vector3D::RandomRange(Vector3D(30, 30, 30), Vector3D(255, 255, 255));
}

void FracturePiece3D::Draw()
{
	//if (bIsHidden) return;
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


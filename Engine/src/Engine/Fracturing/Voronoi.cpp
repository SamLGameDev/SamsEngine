// DO NOT MARK except FracturePlaneRandom, CreateMeshFractureGPU, FracturePlaneRandomGPU, GenerateRandomPointsInBounds, GenerateNewPointSets, FracturePeiceGPU, TetrahedraliseMesh, GenerateCellGPU, GenerateClippedCellGPU, CleanUpBuffers, DrawFractures, LoadPoints, SetUpComputeShaders, CopyBoundingBoxToBuffer, CleanupGPUGeneration
//This is because it has been submitted for my COMP305. Link to Original: https://github.falmouth.ac.uk/GA-Undergrad-Student-Work-25-26/Comp305-Engine-SL295211.git
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


void Voronoi::FracturePlaneRandom(Model& InModel, const size_t& NumPoints, const size_t& PointSetIndex)
{
	Array<Vector3D> points;

	const Vector2D range = { PointSetIndex * NumPoints, ((PointSetIndex + 1) * NumPoints) };

	std::string DataToLoad = "/ExperimentData/SetOf" + std::to_string(NumPoints) + ".txt";

	Fractures.ReSize(NumPoints);

	UFileWriter::Load(DataToLoad, points, range);

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

		}

		FracturePiece3D frac = CreateObjectRaw<FracturePiece3D>(Faces, currentPoint);
		frac.transform = FracturePositions;
		Fractures.Add(frac );
	}

}

void Voronoi::GenerateCellGPU(VoronoiSSBOIn* Buffer, const Array<Vector3D>& Points, const GLuint& VoronoiIn, const GLuint& VoronoiOut, const GLuint&
                              WBuffer, double& TimeBeforeComputation, UComputeShader& VoronoiCompute)
{
	VoronoiCompute.Use();

	::DataBuffers::BindShaderStorageBuffer(VoronoiIn, 1, sizeof(VoronoiSSBOIn));


	::DataBuffers::BindShaderStorageBuffer(VoronoiOut, 3, sizeof(VOut));

	::DataBuffers::BindShaderStorageBuffer(WBuffer, 6, sizeof(WorkingBuffer));

	void* inPtr = ::DataBuffers::MapBufferMemory(VoronoiIn, sizeof(VoronoiSSBOIn));

	VoronoiSSBOIn* inData = static_cast<VoronoiSSBOIn*>(inPtr);

	memcpy(inData, Buffer, sizeof(VoronoiSSBOIn));

	::DataBuffers::UnMapBufferMemory(VoronoiIn);

	void* outPtr = ::DataBuffers::MapBufferMemory(VoronoiOut, sizeof(VOut));

	VOut* outData = static_cast<VOut*>(outPtr);

	outData->NumCells = 0;

	::DataBuffers::UnMapBufferMemory(VoronoiOut);

	TimeBeforeComputation = glfwGetTime();

	VoronoiCompute.Dispatch(Points.GetSize(), 1, 1);
	VoronoiCompute.WaitForCompletion();

	TimeBeforeComputation = glfwGetTime() - TimeBeforeComputation;
}

void Voronoi::GenerateClippedCellGPU(const Array<Vector3D>& Points, const InTets& Tets, const GLuint& VoronoiOut, const GLuint& ClippedOutInd, const GLuint&
                                     InTetsInd, const GLuint& WBuffer, double& TimeBeforeComputation, UComputeShader& ClippingCompute)
{
	ClippingCompute.Use();
	::DataBuffers::BindShaderStorageBuffer(VoronoiOut, 3, sizeof(VOut));

	::DataBuffers::BindShaderStorageBuffer(WBuffer, 6, sizeof(WorkingBuffer));

	::DataBuffers::BindShaderStorageBuffer(InTetsInd, 4, sizeof(InTets));

	::DataBuffers::BindShaderStorageBuffer(ClippedOutInd, 5, sizeof(VOutRaw));

	void* outPtr = ::DataBuffers::MapBufferMemory(ClippedOutInd, sizeof(VOutRaw));

	VOutRaw* outData = static_cast<VOutRaw*>(outPtr);

	outData->NumCells = 0;

	::DataBuffers::UnMapBufferMemory(ClippedOutInd);

	TimeBeforeComputation = glfwGetTime();

	ClippingCompute.Dispatch(Points.GetSize(), 1, 1);
	ClippingCompute.WaitForCompletion();

	TimeBeforeComputation = glfwGetTime() - TimeBeforeComputation;
}

void Voronoi::CleanUpBuffers(const GLuint& VoronoiIn, const GLuint& VoronoiOut, const GLuint& ClippedOutInd, const GLuint& InTetsInd, const GLuint&
                             WBuffer)
{
	::DataBuffers::RemoveBuffer(VoronoiIn);
	::DataBuffers::RemoveBuffer(VoronoiOut);
	::DataBuffers::RemoveBuffer(InTetsInd);
	::DataBuffers::RemoveBuffer(ClippedOutInd);
	::DataBuffers::RemoveBuffer(WBuffer);
}

void Voronoi::DrawFractures(const Array<Vector3D>& points, const GLuint& ClippedOutInd)
{
	void* ptr = ::DataBuffers::MapBufferMemory(ClippedOutInd, sizeof(VOutRaw));

	VOutRaw* data = static_cast<VOutRaw*>(ptr);

	for (size_t i = 0; i < points.GetSize(); i++)
	{
		if (data->CutCells[i].NumInds == 0)
		{
			continue;
		}

		GPUFractures.Add(FracturePieceGPU(data->CutCells[i], points[i]));
		GPUFractures.GetLastPtr()->transform = FracturePositions;
	}
	::DataBuffers::UnMapBufferMemory(ClippedOutInd);
}

void Voronoi::CreateMeshFractureGPU(VoronoiSSBOIn* Buffer, const Array<Vector3D>& Points, const InTets& Tets, const GLuint& VoronoiIn, const GLuint&
                                    VoronoiOut,
                                    const GLuint& ClippedOutInd, const GLuint& InTetsInd, const GLuint& WBuffer,  UComputeShader& VoronoiCompute, UComputeShader& ClippingCompute)
{

	double TimeBeforeComputation;

	GenerateCellGPU(Buffer, Points, VoronoiIn, VoronoiOut, WBuffer, TimeBeforeComputation, VoronoiCompute);

	GenerateClippedCellGPU(Points, Tets, VoronoiOut, ClippedOutInd, InTetsInd, WBuffer, TimeBeforeComputation, ClippingCompute);

	DrawFractures(Points, ClippedOutInd);

}

void Voronoi::CreateMeshFractureGPU(VoronoiSSBOIn* Buffer, const Array<Vector3D>& Points, const InTets& Tets, const GLuint& VoronoiIn, const GLuint&
                                    VoronoiOut,
                                    const GLuint& ClippedOutInd, const GLuint& InTetsInd, const GLuint& WBuffer, PointEntry& Entry, UComputeShader&
                                    VoronoiCompute, UComputeShader& ClippingCompute)
{

	double TimeBeforeComputation;

	GenerateCellGPU(Buffer, Points, VoronoiIn, VoronoiOut, WBuffer, TimeBeforeComputation, VoronoiCompute);

	Entry.Generation = std::to_string(TimeBeforeComputation);

	GenerateClippedCellGPU(Points, Tets, VoronoiOut, ClippedOutInd, InTetsInd, WBuffer, TimeBeforeComputation, ClippingCompute);

	Entry.Clipping = std::to_string(TimeBeforeComputation);
}

void Voronoi::TetrahedraliseMesh(const Model& InModel, InTets& Tets)
{
	for (const auto& subMesh : InModel.Meshes) {

		std::vector<K::Point_3> dtPoints;
		dtPoints.reserve(subMesh.Vertices.GetSize());

		std::vector<std::vector<uint16_t>> inds;
		inds.reserve(subMesh.Indices.GetSize() / 3);

		for (size_t i = 0; i + 2 < subMesh.Indices.GetSize(); i += 3)
		{
			std::vector<uint16_t> tri = { subMesh.Indices[i], subMesh.Indices[i + 1], subMesh.Indices[i + 2] };

			inds.push_back(tri);
		}

		for (const auto& p : subMesh.Vertices)
		{
			dtPoints.emplace_back(p.Position.X, p.Position.Y, p.Position.Z);
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

			Tets.Tets[Tets.NumTets].TetFaces[0] = { v0, v1, v2 };
			Tets.Tets[Tets.NumTets].TetFaces[1] = { v0, v1, v3 };
			Tets.Tets[Tets.NumTets].TetFaces[2] = { v0, v2, v3 };
			Tets.Tets[Tets.NumTets].TetFaces[3] = { v1, v2, v3 };
			Tets.NumTets++;

		}
	}
}

void Voronoi::CopyBoundingBoxToBuffer(const Model& InModel, VoronoiSSBOIn* Buffer)
{
	for (size_t i = 0; i < InModel.BoundingBox->Faces.GetSize(); i++)
	{
		const Face& face = InModel.BoundingBox->Faces[i];
		for (size_t j = 0; j < face.Vertices.GetSize(); j++)
		{
			Buffer->BoundingBoxFaces[i].Verts[j] = face.Vertices[j];
		}
		Buffer->BoundingBoxFaces[i].NumVerts = face.Vertices.GetSize();
	}
}

void Voronoi::SetUpComputeShaders(const InTets& Tets, GLuint& VoronoiIn, GLuint& VoronoiOut, GLuint& ClippedOutInd,
                                  GLuint& InTetsInd, GLuint& WBuffer, UComputeShader& ClippingCompute, UComputeShader
                                  & VoronoiCompute)
{
	::DataBuffers::GenBuffer(VoronoiIn);
	::DataBuffers::GenBuffer(VoronoiOut);
	::DataBuffers::GenBuffer(ClippedOutInd);
	::DataBuffers::GenBuffer(InTetsInd);
	::DataBuffers::GenBuffer(WBuffer);

	VoronoiCompute = UComputeShader("VoronoiCellGeneration", "/Shaders/Voronoi/");
	ClippingCompute = UComputeShader("VoronoiClipping", "/Shaders/Voronoi/");

	VoronoiCompute.Use();
	::DataBuffers::GenerateShaderStorageBuffer(VoronoiIn, sizeof(VoronoiSSBOIn), 1);
	::DataBuffers::GenerateShaderStorageBuffer(VoronoiOut, sizeof(VOut), 3);
	::DataBuffers::GenerateShaderStorageBuffer(WBuffer, sizeof(WorkingBuffer), 6);
	ClippingCompute.Use();
	::DataBuffers::GenerateShaderStorageBuffer(InTetsInd, sizeof(InTets), 4);
	::DataBuffers::GenerateShaderStorageBuffer(ClippedOutInd, sizeof(VOutRaw), 5);
	void* inTetPtr = ::DataBuffers::MapBufferMemory(InTetsInd, sizeof(InTets));

	InTets* inTetsData = static_cast<InTets*>(inTetPtr);

	memcpy(inTetsData, &Tets, sizeof(InTets));

	::DataBuffers::UnMapBufferMemory(InTetsInd);
}

void Voronoi::LoadPoints(VoronoiSSBOIn* InBuffer, Array<Vector3D>& Points, const size_t& NumPoints, const std::string& DataToLoad, const size_t&
                         PointSetIndex)
{
	const Vector2D range = { PointSetIndex * NumPoints, ((PointSetIndex + 1) * NumPoints) };

	UFileWriter::Load(DataToLoad, Points, range);
	for (size_t i = 0; i < Points.GetSize(); i++)
	{
		InBuffer->Points[i] = Points[i];
	}
	InBuffer->NumPoints = Points.GetSize();
}

void Voronoi::FracturePlaneRandomGPU(Model& InModel)
{

	InTets tets;
	tets.NumTets = 0;

	TetrahedraliseMesh(InModel, tets);

	VoronoiSSBOIn* buffer = new VoronoiSSBOIn;
	VOut* vOut = new VOut;
	VOutRaw* ClippedOut = new VOutRaw;

	vOut->NumCells = 0;
	ClippedOut->NumCells = 0;

	CopyBoundingBoxToBuffer(InModel, buffer);

	GLuint VoronoiIn, VoronoiOut, ClippedOutInd, InTetsInd, wBuffer;

	UComputeShader clippingCompute, voronoiCompute;
	SetUpComputeShaders(tets, VoronoiIn, VoronoiOut, ClippedOutInd, InTetsInd, wBuffer, clippingCompute, voronoiCompute);

	DataRecord record;

	record.CPU = UHardwareDetails::GetCPU();
	record.API = UHardwareDetails::API;
	record.Card = UHardwareDetails::GetGPU();
	record.OS = UHardwareDetails::GetOS();
	record.RAM = UHardwareDetails::GetRAM_GB();
	record.PointInfo.ReSize(100 * 145);

	Array<Vector3D> points;
	for (size_t i = 2; i < 101; i++)
	{
		std::string DataToLoad = "/ExperimentData/SetOf" + std::to_string(i) + ".txt";
		PointEntry entry;
		entry.NumPoints = std::to_string(i);

		for (size_t j = 0; j < 145; j++)
		{
			LoadPoints(buffer, points, i, DataToLoad, j);

			CreateMeshFractureGPU(buffer, points, tets, VoronoiIn, VoronoiOut, ClippedOutInd, InTetsInd, wBuffer,
				entry, voronoiCompute, clippingCompute);

			record.PointInfo.Add(entry);

		}
	}
	DataRecorder::SaveDataRecord(record, "/ExperimentData/TestData.json");
	CleanupGPUGeneration(buffer, vOut, ClippedOut, VoronoiIn, VoronoiOut, ClippedOutInd, InTetsInd, wBuffer);

}

void Voronoi::CleanupGPUGeneration(VoronoiSSBOIn* InBuffer, VOut* VoronoiOutBuffer, VOutRaw* ClippedOut, GLuint& VoronoiIn, GLuint& VoronoiOut, GLuint&
                                   ClippedOutInd, GLuint& InTetsInd, GLuint& wBuffer)
{
	CleanUpBuffers(VoronoiIn, VoronoiOut, ClippedOutInd, InTetsInd, wBuffer);
	delete InBuffer;
	delete VoronoiOutBuffer;
	delete ClippedOut;
}

void Voronoi::FracturePlaneRandomGPU(Model& InModel, const size_t& NumPoints, const size_t& PointSetIndex)
{

	InTets tets;
	tets.NumTets = 0;

	TetrahedraliseMesh(InModel, tets);

	VoronoiSSBOIn* buffer = new VoronoiSSBOIn;
	VOut* vOut = new VOut;
	VOutRaw* ClippedOut = new VOutRaw;

	vOut->NumCells = 0;
	ClippedOut->NumCells = 0;

	CopyBoundingBoxToBuffer(InModel, buffer);

	GLuint VoronoiIn, VoronoiOut, ClippedOutInd, InTetsInd, wBuffer;

	UComputeShader clippingCompute, voronoiCompute;
	SetUpComputeShaders(tets, VoronoiIn, VoronoiOut, ClippedOutInd, InTetsInd, wBuffer, clippingCompute, voronoiCompute);

	std::string DataToLoad = "/ExperimentData/SetOf" + std::to_string(NumPoints) + ".txt";

	Array<Vector3D> points;

	LoadPoints(buffer, points, NumPoints, DataToLoad, PointSetIndex);

	CreateMeshFractureGPU(buffer, points, tets, VoronoiIn, VoronoiOut, ClippedOutInd, InTetsInd, wBuffer, voronoiCompute, clippingCompute);
	CleanupGPUGeneration(buffer, vOut, ClippedOut, VoronoiIn, VoronoiOut, ClippedOutInd, InTetsInd, wBuffer);
}

Array<Vector3D> Voronoi::GenerateRandomPointsInBounds(const Model& InModel, const size_t& NumPoints, Array<Vector3D>& Points)
{
	Points.Reallocate(NumPoints);

	for (size_t i = 0; i < NumPoints; i++)
	{

		Vector3D point1;
		do
		{
			point1 = InModel.ModelTransform.GetRandomPointInBounds();
		} while (Points.Contains(point1) || IsPointTooClose(point1, Points));

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


void Voronoi::GenerateVoronoiCellsDelaunay(const Array<Vector3D>& Points, const Model& InModel)
{
	
	DelaunayTriangulation dt;

	dt.Triangulate(Points);
	for (const auto& point : Points)
	{
		GenerateVoronoiCellDelaunay(InModel, dt.Tetrahedrons, point);
	}

}

void Voronoi::GenerateNewPointSets(const Model& InModel)
{
	for (size_t i = 2; i < 101; i++)
	{
		Array<Vector3D> pointSet;
		pointSet.ReSize(i * 145);
		Array<Vector3D> points;
		for (size_t j = 0; j < 145; j++)
		{
			GenerateRandomPointsInBounds(InModel, i, points);
			pointSet.Emplace(std::move(points));
		}
		UFileWriter::SaveArray("/ExperimentData/SetOf" + std::to_string(i) + ".txt", pointSet);
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

FracturePieceGPU::FracturePieceGPU(Cell& InVoronoiOut, const Vector3D& InPoint)
{
	for (size_t j = 0; j < InVoronoiOut.NumFaces; j++)
	{
		FixedSizeFace* face = &InVoronoiOut.Faces[j];
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
	SetupControls(Point);

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
	shader = Shader("ColorShape", "/Shaders/");

	TriangulateCell(CellFaces);

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

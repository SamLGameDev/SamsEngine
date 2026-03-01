#include "VoronoiClipping.h"

#include <iostream>

#include "AABB.h"
#include "CollisionCast.h"
#include "DataBuffers.h"
#include "MathCore.h"
#include "ObjectFactory.h"
#include "SutherlandHodegman.h"
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_3.h>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/IO/polygon_mesh_io.h>
#include <CGAL/Surface_mesh.h>
#include <CGAL/make_conforming_constrained_Delaunay_triangulation_3.h>
#include <CGAL/IO/write_MEDIT.h>

#include <cassert>

#include "DelaunayTriangulation.h"
#include "PlaneClipping.h"
#include "Predictates.h"
using Kernel = CGAL::Exact_predicates_inexact_constructions_kernel;
using K = CGAL::Exact_predicates_inexact_constructions_kernel;
using Point = Kernel::Point_3;
using DT = CGAL::Delaunay_triangulation_3<Kernel>;

FracturedMeshPiece::~FracturedMeshPiece()
{
	::Renderer::RemoveMeshFracturePiece(this);
}

FracturedMeshPiece::FracturedMeshPiece(const Array<Face>& cell, const Vector3D& Point)
{
	std::cout << "Creating fractured mesh piece with " << cell.GetSize() << " faces." << std::endl;

	Verts.ReSize(cell.GetSize() * 10);
	Inds.ReSize(cell.GetSize() * 10);

	SetupControls(Point);

	shader = Shader("ColorShape", "/Shaders/");

	TriangulateCell(cell);

	transform.Position = { 0, 0, 0 };

	if (Inds.IsEmpty())
	{
		return;
	}

	BufferData();
}

FracturedMeshPiece::FracturedMeshPiece(const Array<Vector3D>& InVerts, const Array<uint16_t>& InInds, const Vector3D& Point)
{
	Verts = InVerts;
	Inds = InInds;

	SetupControls(Point);

	shader = Shader("ColorShape", "/Shaders/");

	//TriangulateCell(cell);

	transform.Position = { 0, 0, 0 };

	if (Inds.IsEmpty())
	{
		return;
	}

	BufferData();
}

FracturedMeshPiece::FracturedMeshPiece(const Array<FTriangle>& cell, const Vector3D& Point)
{
	SetupControls(Point);

	shader = Shader("ColorShape", "/Shaders/");

	TriangulateCell(cell);

	if (Inds.IsEmpty())
	{
		return;
	}

	BufferData();
}

void FracturedMeshPiece::SetupControls(const Vector3D& point)
{
	InputManager* inputManager = Camera::GetActiveCamera()->GetActiveInputManager();
	LeftArrow = std::make_unique<InputAction>(GLFW_KEY_LEFT, inputManager, Camera::GetActiveWindow());

	LeftArrow->Actions.BindMember(this, &FracturedMeshPiece::Seperate);

	RightArrow = std::make_unique<InputAction>(GLFW_KEY_RIGHT, inputManager, Camera::GetActiveWindow());

	RightArrow->Actions.BindMember(this, &FracturedMeshPiece::Converge);

	dir = (point - Vector3D::Zero).Normalised();

	this->Point = point;
}

void FracturedMeshPiece::Seperate()
{

	transform.Position += (dir * 5) * World->GetDeltaTime();

}
void FracturedMeshPiece::Converge()
{
	transform.Position -= (dir * 5) * World->GetDeltaTime();
}

void FracturedMeshPiece::ToggleHidden()
{
	if (bHidable)
	{
		bHidden = !bHidden;
	}
}

void FracturedMeshPiece::TriangulateCell(const Array<Face>& cell)
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

void FracturedMeshPiece::TriangulateCell(const Array<FTriangle>& cell)
{
	for (const auto& tri : cell)
	{
		AddOrMakeInd(tri[0]);
		AddOrMakeInd(tri[1]);
		AddOrMakeInd(tri[2]);
	}
}

void FracturedMeshPiece::AddOrMakeInd(const Vector3D& Vert)
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

void FracturedMeshPiece::BufferData()
{
	::DataBuffers::GenBuffer(VAO);

	DataBuffers::BindVertexInfo(VAO, 0, 0, sizeof(Vector3D), 0, Vector3);

	::DataBuffers::BufferData(VAO, Verts.GetSize() * sizeof(Vector3D), Verts.GetFirstPtr(), BufferTargets::VERTEX);
	DataBuffers::BufferDataIndex(VAO, Inds.GetSize() * sizeof(uint16_t), Inds.GetFirstPtr());

	::Renderer::AddMeshFracturePiece(this);
}
void FracturedMeshPiece::Draw()
{
	//if (bHidden) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	//else glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	shader.Use();

	DataBuffers::BindBuffer(VAO);

	DataBuffers::DrawVertexData(VAO);

	PerInstanceTransforms ubo;
	ubo.Model = transform.GetModelMatrix();
	ubo.Color = Color;

	GlobalTransforms g;
	g.View = Camera::GetActiveCamera()->GetLook();

	g.Projection = Camera::GetActiveCamera()->GetProjection();

	shader.SetUniformBuffer(0, &g, sizeof(GlobalTransforms));

	shader.SetUniformBuffer(1, &ubo, sizeof(PerInstanceTransforms));

	::Renderer::Draw(Inds.GetSize());

}

void FracturedMeshPiece::Start()
{
	WorldObject::Start();
}

void FracturedMeshPiece::Tick(const double& DeltaTime)
{
	WorldObject::Tick(DeltaTime);
}


void VoronoiClipping::ClipCellToMesh(Array<FTetrahedron>& tets, const FracturePiece3D& cell)
{
	FBox cellBox(cell.Verts);
	Array<Face> newCell;
	newCell.ReSize(tets.GetSize() / 2);

	Array<Vector3D> Verts;
	Array<uint16_t> Inds;

	for (auto& tet : tets)
	{

		if (!AABB::IsBoxIntersectingBox(cellBox, FBox(tet.Verts))) continue;

		Array<Face> copyFaces = tet.Faces;
		PlaneClipping::ClipCellByFaces(copyFaces, cell.CellFaces);

		if (copyFaces.IsEmpty()) continue;

		for (const auto& face : copyFaces)
		{
			for (size_t j = 1; j + 1 < face.Vertices.GetSize(); j++)
			{
				AddOrMakeInd(Verts, Inds, face.Vertices[0]);
				AddOrMakeInd(Verts, Inds, face.Vertices[j]);
				AddOrMakeInd(Verts, Inds, face.Vertices[j + 1]);
			}
		}

		newCell.Emplace(std::move(copyFaces));

	}

	if (Verts.IsEmpty() || newCell.IsEmpty()) return;

	std::scoped_lock lock(VoronoiMutex);
	Vector3D color = Vector3D::RandomRange(Vector3D(30, 30, 30), Vector3D(255, 255, 255));
	FracturedMeshPiece frac = CreateObjectRaw<FracturedMeshPiece>(Verts, Inds, cell.Point);
	frac.Color = color;
	FracturedPieces.Emplace(std::move(frac));
}

void VoronoiClipping::ClipCellToMesh(const Array<Face>& meshFaces, const Vector3D& MeshCenter, FracturePiece3D& cell)
{
	FBox cellBox(cell.Verts);
	Array<Face> newCell = cell.CellFaces;

	Array<Vector3D> normals;
	Array<Vector3D> centers;

	Array<Array<Face>> cellStages; //=  {cell.CellFaces};

	bool bNoClip = true;

	for (size_t i = 0; i < meshFaces.GetSize(); i++)
	{

		if (!AABB::IsBoxIntersectingBox(cellBox, FBox(meshFaces[i].Vertices))) continue;

		Face test = meshFaces[i];
		Face intersectFace;
		Vector3D cellCenter = Vector3D::Zero;
		for (const auto& plane : newCell)
		{
			cellCenter += plane.GetCenter();
		}
		cellCenter = cellCenter / newCell.GetSize();

		for (const auto& plane : newCell)
		{

			Vector3D normal = Vector3D::GetPlaneNormal(plane.Vertices, Vector3D::Zero);


			Vector3D center = plane.GetCenter();

			if (!MathCore::IsNearlyZero(Vector3D::Dot(normal,  cellCenter - center)) &&
				Vector3D::Dot(normal, cellCenter - center) >= 0)
			{
				normal = -normal;
			}

			if (Vector3D::IsAlmostEqual(normal, Vector3D::Zero))
			{
				std::cout << "Plane normal is zero, skipping clipping for this plane." << std::endl; continue;
			}

			Face f;

			PlaneClipping::ClipFaceByFace(test, center, f, normal, intersectFace);

			test = f;

			if (i == 7)
			{

				Vector3D right = Vector3D::Cross(normal, Vector3D(0, 1, 0));
				Vector3D up = Vector3D::Cross(right, normal);

				Array<Face> copyFaces = { {{center + ((right + up) ), center + ((right + -up) ), center - ((right + up) ), center + ((-right + up) ), center + normal, center + normal * 3}} };
				cellStages.Add(copyFaces);
			}

			if (test.Vertices.IsEmpty())
			{
				break;
			}
			intersectFace.Vertices.Empty();
		}
		
		if (test.Vertices.IsEmpty()) continue;

		bNoClip = false;

		Vector3D normal = Vector3D::GetPlaneNormal(meshFaces[i].Vertices, Vector3D::Zero);

		Vector3D center = meshFaces[i].GetCenter();

		if (!MathCore::IsNearlyZero(Vector3D::Dot(normal, MeshCenter - center)) &&
			Vector3D::Dot(normal, MeshCenter - center) >= 0)
		{
			normal = -normal;
		}

		if (Vector3D::IsAlmostEqual(normal, Vector3D::Zero))
		{
			std::cout << "Plane normal is zero, skipping clipping for this plane." << std::endl; continue;
		}
		normals.Add(normal);
		centers.Add(center);

		cellStages.Add({ test });

		//Array<Face> copyFaces = tet.Faces;
		PlaneClipping::ClipCellByFace(newCell, center, normal);

		for (const auto& plane : newCell)
		{
			if (plane.Vertices.Contains(Vector3D::Zero))
			{
				std::cout << "Cell already contains the plane, skipping." << std::endl;
				continue;
			}
		}

	

		if (newCell.IsEmpty())
		{

			Vector3D right = Vector3D::Cross(normal, Vector3D(0, 1, 0));
			Vector3D up = Vector3D::Cross(right, normal);

			Array<Face> copyFaces = {{{center, center + ((right + up) * 10), center - ((right + up) * 10), center + ((right + -up) * 10), center + ((-right + up) * 10), center + normal * 10, center + normal * 100}}};
			cellStages.Add(copyFaces);
			break;
		}

		for (auto& face : newCell)
		{
		//	Vector3D::OrderByAngle(face.Vertices, face.GetCenter(), normal);
		}
	//	cellStages.Add(newCell);

		Vector3D right = Vector3D::Cross(normal, Vector3D(0, 1, 0));
		Vector3D up = Vector3D::Cross(right, normal);

		Array<Face> copyFaces = { {{center + ((right + up) * 10), center + ((right + -up) * 10), center - ((right + up) * 10), center + ((-right + up) * 10), center + normal * 10, center + normal * 100}} };
		cellStages.Add(copyFaces);
		//break;
		//if (newCell.IsEmpty())
		//{

		//	for (size_t j = 0; j < normals.GetSize(); j++)
		//	{
		//		std::cout << "Normal: " << normals[j] << " Center: " << centers[j] << std::endl;
		//		for (const auto& stage : cellStages[j])
		//		{
		//			std::cout << "Stage Face: " << std::endl;
		//			for (const auto& vert : stage.Vertices)
		//			{
		//				std::cout << vert << std::endl;
		//			}

		//		}
		//	}
		//	std::cout << "ended" << "\n";
		//}

	}

	//if (newCell == cell.CellFaces) return;

	//for (size_t i = 0; i < cellStages.GetSize(); i++)
	//{
	//	std::scoped_lock lock(VoronoiMutex);
	//	Vector3D color = Vector3D::RandomRange(Vector3D(30, 30, 30), Vector3D(255, 255, 255));
	//	FracturedMeshPiece frac = CreateObjectRaw<FracturedMeshPiece>(cellStages[i], cell.Point);
	//	frac.Color = color;
	//	//frac.transform.Position = MeshCenter;
	//	FracturedPieces.Emplace(std::move(frac));
	//}

	if (bNoClip)
	{
		for (const auto& plane : meshFaces)
		{
			Vector3D normal = Vector3D::GetPlaneNormal(plane.Vertices, Vector3D::Zero);

			Vector3D center = plane.GetCenter();

			if (!MathCore::IsNearlyZero(Vector3D::Dot(normal, MeshCenter - center)) &&
				Vector3D::Dot(normal, MeshCenter - center) >= 0)
			{
				normal = -normal;
			}

			if (Vector3D::IsAlmostEqual(normal, Vector3D::Zero))
			{
				std::cout << "Plane normal is zero, skipping clipping for this plane." << std::endl; continue;
			}

			if (Vector3D::Dot(normal, cell.Point - center) > 0)
			{
				std::cout << "Cell is outside the mesh, skipping." << std::endl;
				return;
			}

		}
	}

	std::scoped_lock lock(VoronoiMutex);
	Vector3D color = Vector3D::RandomRange(Vector3D(30, 30, 30), Vector3D(255, 255, 255));
	FracturedMeshPiece frac = CreateObjectRaw<FracturedMeshPiece>(newCell, cell.Point);
	frac.Color = cell.color;
	frac.bHidden= true;
	FracturedPieces.Emplace(std::move(frac));

	Vector3D color2 = Vector3D::RandomRange(Vector3D(30, 30, 30), Vector3D(255, 255, 255));
	FracturedMeshPiece frac2 = CreateObjectRaw<FracturedMeshPiece>(cell.CellFaces, cell.Point);
	frac2.Color = cell.color;
	//frac2.transform.Position = Vector3D(3, 0, 0);
	frac2.bHidden = false;
	FracturedPieces.Emplace(std::move(frac2));

}

void VoronoiClipping::ClipMeshToVoronoi(Voronoi& Diagram, const Model& Mesh)
{
	Array<FTetrahedron> tets =  TetrahredraliseMeshCGAL(Mesh);
	//Array<std::jthread> threads;
	for (const auto& cell : Diagram.Fractures)
	{
		//std::jthread thread(&VoronoiClipping::ClipCellToMesh, this, std::ref(tets), std::ref(cell));
		//threads.Emplace(std::move(thread));
		ClipCellToMesh(tets, cell);
	}
}

void VoronoiClipping::ClipTriangleMeshToVoronoi(Voronoi& Diagram, const Model& Mesh)
{
	//Array<std::jthread> threads;

	Array<Vector3D> verts;
	Array<uint16_t> inds;
	for (const auto& subMesh : Mesh.Meshes)
	{
		verts.ReSize(subMesh.Vertices.GetSize());
		for (size_t i = 0; i < subMesh.Vertices.GetSize(); i++)
		{
			verts[i] = subMesh.Vertices[i].Position;
		}
		inds.Reallocate(inds.GetSize() + subMesh.Indices.GetSize());
		memcpy(inds.GetFirstPtr(), subMesh.Indices.GetFirstPtr(), subMesh.Indices.GetSize() * sizeof(uint16_t));
	}
	for (size_t i = 6; i < 7; i++)
	{
		//std::jthread thread(&VoronoiClipping::ClipCellToMesh, this, std::ref(tets), std::ref(cell));
		//threads.Emplace(std::move(thread));
		ClipCellToMesh(Mesh.Meshes[0].Faces, Mesh.ModelTransform.Center, Diagram.Fractures[i]);
		//return;
	}
}

void VoronoiClipping::AddOrMakeInd(Array<Vector3D>& Verts, Array<uint16_t>& Inds, const Vector3D& Vert)
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

Array<FTetrahedron> VoronoiClipping::TetrahredraliseMesh(const Model& Mesh)
{
	Array<FTetrahedron> tetrahedra;
	for (const auto& subMesh : Mesh.Meshes) {

		DelaunayTriangulation dt;
		Array<Vector3D> points;
		points.Reallocate(subMesh.Vertices.GetSize());
		for (size_t i= 0; i < subMesh.Vertices.GetSize(); i++)
		{
			points[i] = subMesh.Vertices[i].Position;
		}

		dt.Triangulate(points);

		dt.RemoveSuperTriangle();

		for (const auto& tet : dt.Tetrahedrons)
		{
			tetrahedra.Add({ tet.point1, tet.point2, tet.point3, tet.point4 });
		}
	} 
	return tetrahedra;

}
Array<FTetrahedron> VoronoiClipping::TetrahredraliseMeshCGAL(const Model& Mesh) {
	Array<FTetrahedron> tetrahedra;
	tetrahedra.ReSize(Mesh.Meshes.GetSize() * 20000); 
	FracturedPieces.ReSize(40000);// Arbitrary number, should be enough for most meshes, but can be changed if needed
	for (const auto& subMesh : Mesh.Meshes) {

		//CGAL::Surface_mesh<K::Point_3> dtPoints;
		std::vector<K::Point_3> dtPoints;
		dtPoints.reserve(subMesh.Vertices.GetSize());

		//if (!CGAL::IO::read_polygon_mesh("D:/Comp303-SL295211-VoronoiClipping/Engine/Contents/Models/Bunny/Bunny.obj", dtPoints)) {
		//	std::cerr << "Error: cannot read file "  << std::endl;
		//}

		std::vector<std::vector<uint16_t>> inds;
		inds.reserve(subMesh.Indices.GetSize() / 3);

		for (size_t i = 0; i + 2< subMesh.Indices.GetSize(); i+=3)
		{
			std::vector<uint16_t> tri = { subMesh.Indices[i], subMesh.Indices[i+1], subMesh.Indices[i+2] };

			inds.push_back(tri);
		}

		for (const auto& p : subMesh.Vertices)
		{
			dtPoints.push_back({ p.Position.X, p.Position.Y, p.Position.Z });
		}

		auto dt = CGAL::make_conforming_constrained_Delaunay_triangulation_3(dtPoints, inds);


		//DT dt;
		//dt.insert(dtPoints.begin(), dtPoints.end());
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
			tetrahedra.Add({ v0, v1, v2, v3 });

		}
	}
	return tetrahedra;


}

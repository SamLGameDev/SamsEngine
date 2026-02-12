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

#include "PlaneClipping.h"
#include "Predictates.h"
using Kernel = CGAL::Exact_predicates_inexact_constructions_kernel;
using Point = Kernel::Point_3;
using DT = CGAL::Delaunay_triangulation_3<Kernel>;

FracturedMeshPiece::FracturedMeshPiece(const Array<Face>& cell, const Vector3D& Point)
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
	//if (bHidden) return;

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


void VoronoiClipping::ClipMeshToVoronoi(Voronoi& Diagram, const Model& Mesh)
{
	Array<FTetrahedron> tets =  TetrahredraliseMesh(Mesh);

	for (const auto& cell : Diagram.Fractures)
	{
		FBox cellBox(cell.Verts);

		Array<Face> newCell;

		std::cout << "NewCell ------------------------------------------------------------------------------------------------ \n";

		for (auto& tet : tets)
		{

			if (!AABB::IsBoxIntersectingBox(cellBox, FBox(tet.Verts))) continue;

			Array<Face> copyFaces = tet.Faces;
			PlaneClipping::ClipCellByFaces(copyFaces, cell.CellFaces);

			newCell.Add(copyFaces);
			
		}

		Vector3D color = Vector3D::RandomRange(Vector3D(30, 30, 30), Vector3D(255, 255, 255));
		FracturedMeshPiece frac = CreateObjectRaw<FracturedMeshPiece>(newCell, Vector3D::Zero);
		frac.Color = color;
		FracturedPieces.Emplace(std::move(frac));
	}
}

Array<FTetrahedron> VoronoiClipping::TetrahredraliseMesh(const Model& Mesh)
{

	Array<Point> dtPoints;

	for (const auto& p : Mesh.Meshes[0].Vertices)
	{
		dtPoints.Add({ p.Position.X, p.Position.Y, p.Position.Z });
	}

	DT dt;
	dt.insert(dtPoints.begin(), dtPoints.end());

	Array<FTetrahedron> tetrahedra;

	for (auto cell = dt.finite_cells_begin(); cell != dt.finite_cells_end(); ++cell)
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

	return tetrahedra;


}

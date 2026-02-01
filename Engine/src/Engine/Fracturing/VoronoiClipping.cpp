#include "VoronoiClipping.h"

#include <iostream>

#include "AABB.h"
#include "CollisionCast.h"
#include "DataBuffers.h"
#include "MathCore.h"
#include "ObjectFactory.h"
#include "SutherlandHodegman.h"

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
	if (bHidden) return;

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

	size_t numcells = 0;


	Array<FracturePiece3D> Boundary;

	//Find Boundary, make inside cells visable. Disacrd outside cells
	//Remember about the triangle clipping, but having all 3 points outside.
	//AABB but between two bounding boxes would work. 
	for (auto& cell : Diagram.Fractures)
	{


		FBox box = FBox(cell.Verts);

		for (const auto& mesh : Mesh.Meshes)
		{
			for (size_t i = 0; i + 2 < mesh.Indices.GetSize(); i += 3)
			{
				const Vertex& p1 = mesh.Vertices[mesh.Indices[i]];
				const Vertex& p2 = mesh.Vertices[mesh.Indices[i + 1]];
				const Vertex& p3 = mesh.Vertices[mesh.Indices[i + 2]];

				const FBox clippingBox = FBox({ p1.Position, p2.Position, p3.Position });
				if (AABB::IsBoxIntersectingBox(box, clippingBox))
				{
					for (size_t j = 0; j < cell.CellFaces.GetSize(); j++)
					{
						const VoronoiFace& f = cell.CellFaces[j];

						if (f.Vertices.GetSize() < 3) continue;


						Vector3D v1 = f.Vertices[0].point;
						Vector3D v2 = f.Vertices[1].point;
						Vector3D v3 = f.Vertices[2].point;
						Vector3D n = Vector3D::Cross(v2 - v1, v3 - v1).Normalised();
						const double d = Vector3D::Dot(n, (v1 - cell.Point).Normalised());

						if (d < 0) n = -n;

					}
				}

			}
		}
	}

	//Then go through only the boundary cells to clip them.
	//Fix clipping, should be easier with only boundary cells.

	for (auto& cell : Diagram.Fractures)
	{

		Vector3D cen;
		for (const auto& l : cell.Verts)
		{
			cen += l;
		}
		cen = cen / cell.Verts.GetSize();


		FBox box = FBox(cell.Verts);

		Array<FTriangle> InsideTriangles;

		Array<FTriangle> ClippedTriangles;

		uint32_t numTris = 0;

		for (const auto& mesh : Mesh.Meshes)
		{
			for (size_t i = 0; i + 2 < mesh.Indices.GetSize(); i += 3)
			{
				Vertex p1 = mesh.Vertices[mesh.Indices[i]];
				Vertex p2 = mesh.Vertices[mesh.Indices[i + 1]];
				Vertex p3 = mesh.Vertices[mesh.Indices[i + 2]];

				InsideTriangles.Add(FTriangle(p1.Position, p2.Position, p3.Position));

		/*		bool bP1Inside = AABB::IsPointInsideBox(box, p1.Position);
				bool bP2Inside = AABB::IsPointInsideBox(box, p2.Position);
				bool bP3Inside = AABB::IsPointInsideBox(box, p3.Position);

				numTris++;

				if (bP1Inside || bP2Inside || bP3Inside)
				{

					for (size_t j = 0; j < cell.CellFaces.GetSize(); j++)
					{
						VoronoiFace& f = cell.CellFaces[j];

						if (f.Vertices.GetSize() < 3) continue;


						Vector3D v1 = f.Vertices[0].point;
						Vector3D v2 = f.Vertices[1].point;
						Vector3D v3 = f.Vertices[2].point;
						Vector3D n = Vector3D::Cross(v2 - v1, v3 - v1).Normalised();
						float d = Vector3D::Dot(n, (v1 - cen).Normalised());

						if (d < 0) n = -n;

						const double g = -Vector3D::Dot(n, v1);



						const double d1 = Vector3D::Dot(n, v2) + g;
						const double d2 = Vector3D::Dot(n, v3) + g;

						const double d3 = Vector3D::Dot(n, v3) + g;
						const double d4 = Vector3D::Dot(n, v1) + g;

						const double d5 = Vector3D::Dot(n, v1) + g;
						const double d6 = Vector3D::Dot(n, v2) + g;

						const auto inside = [](const double& delta) { return delta <= 0; };
						const auto outside = [](const double& delta) { return delta > 0; };

						if (outside(d1 && outside(d2)))
						{
							bP2Inside = false;
						}

						if (outside(d3) && outside(d4))
						{
							bP3Inside = false;
						}

						if (outside(d5) && outside(d6))
						{
							bP1Inside = false;
						}

					}

					if (bP1Inside || bP2Inside || bP3Inside)
					{
						if (bP1Inside && bP2Inside && bP3Inside)
						{
							InsideTriangles.Add(FTriangle(p1.Position, p2.Position, p3.Position));
							continue;
						}
						ClippedTriangles.Add(FTriangle(p1.Position, p2.Position, p3.Position));
					}

				}*/
			}
		}


		std::cout << "Inside Triangles: " << InsideTriangles.GetSize() << std::endl;
		std::cout << "Clipped Triangles: " << ClippedTriangles.GetSize() << std::endl;
		std::cout << "Total Triangles: " << numTris << std::endl;

		if (InsideTriangles.IsEmpty() && ClippedTriangles.IsEmpty())
		{
			continue;
		}

		Array<Face> ResultFaces;

		Array<FTriangle>ResultTris;
		//ResultFaces.Reallocate(InsideTriangles.GetSize());
		//ResultFaces.Reallocate(InsideTriangles.GetSize() + ClippedTriangles.GetSize());
		//ResultFaces.Reallocate(ClippedTriangles.GetSize());
		Array<FTriangle> ClippingTris;

		for (size_t i = 0; i < ClippedTriangles.GetSize(); i++)
		{
			
			Face outFace;
			SutherlandHodgeman::Clip3D(cell.CellFaces, ClippedTriangles[i], outFace, cen);

			ResultTris.Add(ClippedTriangles[i]);

			if (outFace.Vertices.GetSize() < 3)
			{
				std::cout << "Clipped face has less than 3 verts!" << std::endl;
			};

			for (size_t j = 1; j +1< outFace.Vertices.GetSize(); j++)
			{
				//ResultTris.Add({ outFace.Vertices[0], outFace.Vertices[j], outFace.Vertices[j + 1] });
			//	ClippingTris.Add({ outFace.Vertices[0], outFace.Vertices[j], outFace.Vertices[j + 1] });
			}

			ResultFaces.Add(outFace);

			//outFace.Normal = Vector3D::Cross(
			//	ClippedTriangles[i][1] - ClippedTriangles[i][0],
			//	ClippedTriangles[i][2] - ClippedTriangles[i][0]).Normalised();

		}

		for (size_t i = 0; i < InsideTriangles.GetSize(); i++)
		{
			Face outFace;
			SutherlandHodgeman::Clip3D(cell.CellFaces, InsideTriangles[i], outFace, cen);

			//ResultFaces.Add(outFace);

			for (size_t j = 1; j + 1 < outFace.Vertices.GetSize(); j++)
			{
				ResultTris.Add({ outFace.Vertices[0], outFace.Vertices[j], outFace.Vertices[j + 1] });
			//	ClippingTris.Add({ outFace.Vertices[0], outFace.Vertices[j], outFace.Vertices[j + 1] });
			}

			//ResultFaces.Add( { {InsideTriangles[i].Verts[0], InsideTriangles[i].Verts[1], InsideTriangles[i].Verts[2]} });
			//ResultTris.Add(InsideTriangles[i]);
		}


		if (ResultTris.IsEmpty())
		{
			continue;
		}

		Array<Array<FTriangle>> Shapes;

		while (!ResultTris.IsEmpty())
		{
			bool bFoundShared = false;
			Array<FTriangle> ProcessedTris = ResultTris;
			Array<FTriangle> shape;
			shape.Add(ResultTris[0]);
			ProcessedTris.RemoveAt(0);
			ResultTris.RemoveAt(0);
			size_t numclip = 0;
			while (!bFoundShared) {
				bFoundShared = true;
				for (size_t t = 0; t < shape.GetSize(); t++)
				{
					for (size_t i = 0; i < ProcessedTris.GetSize(); i++)
					{
						if (shape[t].ShareEdge(ProcessedTris[i]))
						{
							shape.Add(ProcessedTris[i]);
							FTriangle trian = ProcessedTris[i];
							ResultTris.RemoveAll(trian);

							bFoundShared = false;
						}
					}

					ProcessedTris = ResultTris;
				}
			}
		//	std::cout << "Clipping Tris in shape: " << numclip << std::endl;
			Shapes.Add(shape);

		}
		//So clip cell planes by each tris normal? 
//Check if part inside tri
		//cell.ToggleRendering();



		for (const auto& shape : Shapes)
		{
			Array<Face> outFace;
			Array<FTriangle> testTris;

			for (size_t i = 0; i + 2 < cell.Inds.GetSize(); i += 3)
			{
				testTris.Add(FTriangle(cell.Verts[cell.Inds[i]], cell.Verts[cell.Inds[i + 1]], cell.Verts[cell.Inds[i + 2]]));
			}

			for (const auto& tri : testTris)
			{
				Face f;
				SutherlandHodgeman::Clip3D(shape, tri, f, Vector3D::Zero);
				outFace.Add(f);
			}

			//SutherlandHodgeman::Clip3D(shape, cell.CellFaces, outFace, Vector3D::Zero);

			for (const auto& tri : shape)
			{
				Face f;
				f.Vertices.Add(tri[0]);
				f.Vertices.Add(tri[1]);
				f.Vertices.Add(tri[2]);
				outFace.Add(f);
			}

			Vector3D color = Vector3D::RandomRange(Vector3D(30, 30, 30), Vector3D(255, 255, 255));
			FracturedMeshPiece frac = CreateObjectRaw<FracturedMeshPiece>(outFace, cen);
			frac.Color = color;
			FracturedPieces.Emplace(std::move(frac));

		//	FracturedMeshPiece frac2 = CreateObjectRaw<FracturedMeshPiece>(ResultTris, cen);
			//frac2.Color = {10, 10, 10};
			//frac2.bHidable = true;
			//FracturedPieces.Emplace(std::move(frac2));
			//cell.ToggleRendering();
		}
		cell.bIsHidden = true;
			//numcells++;

			//if (numcells > 5)
			//{
			//	return;
			//}

			//break;

		//FracturedMeshPiece frac = CreateObjectRaw<FracturedMeshPiece>(ResultFaces, cen);
		//frac.Color = { 50, 50, 0 };
		//FracturedPieces.Emplace(std::move(frac));
	}
}

#include "VoronoiClipping.h"

#include <iostream>

#include "AABB.h"
#include "CollisionCast.h"
#include "SutherlandHodegman.h"

void VoronoiClipping::ClipMeshToVoronoi(const Voronoi& Diagram, const Model& Mesh)
{
	const FracturePiece3D& cell = Diagram.Fractures[0];

	FBox box = FBox(cell.Verts);

	Array<FTriangle> InsideTriangles;

	Array<FTriangle> ClippedTriangles;

	uint32_t numTris = 0;

	for (const auto& mesh : Mesh.Meshes)
	{
		for (size_t i = 0; i+2 < mesh.Indices.GetSize(); i+=3)
		{
			Vertex p1 = mesh.Vertices[mesh.Indices[i]];
			Vertex p2 = mesh.Vertices[mesh.Indices[i+1]];
			Vertex p3 = mesh.Vertices[mesh.Indices[i+2]];

			bool bP1Inside = AABB::IsPointInsideBox(box, p1.Position);
			bool bP2Inside = AABB::IsPointInsideBox(box, p2.Position);
			bool bP3Inside = AABB::IsPointInsideBox(box, p3.Position);

			numTris++;

			if (bP1Inside || bP2Inside || bP3Inside)
			{

				bP1Inside = CollisionCast::RayCastShape(p1.Position, Vector3D::Up, cell.Verts, cell.Inds);
				bP2Inside = CollisionCast::RayCastShape(p2.Position, Vector3D::Up, cell.Verts, cell.Inds);
				bP3Inside = CollisionCast::RayCastShape(p3.Position, Vector3D::Up, cell.Verts, cell.Inds);

				if (bP1Inside || bP2Inside || bP3Inside)
				{
					if (bP1Inside && bP2Inside && bP3Inside)
					{
						InsideTriangles.Add(FTriangle(p1.Position, p2.Position, p3.Position));
						continue;
					}
					ClippedTriangles.Add(FTriangle(p1.Position, p2.Position, p3.Position));
				}
			}


		}
	}
	std::cout << "Inside Triangles: " << InsideTriangles.GetSize() << std::endl;
	std::cout << "Clipped Triangles: " << ClippedTriangles.GetSize() << std::endl;
	std::cout << "Total Triangles: " << numTris << std::endl;

	Array<Face> ResultFaces;
	ResultFaces.Reallocate(ClippedTriangles.GetSize());

	for (size_t i = 0; i < ClippedTriangles.GetSize(); i++)
	{
		Face outFace;
		SutherlandHodgeman::Clip3D(cell.CellFaces, ClippedTriangles[i], outFace, cell.Point);
		ResultFaces[i] = outFace;

		outFace.Normal = Vector3D::Cross(
			ClippedTriangles[i][1] - ClippedTriangles[i][0],
			ClippedTriangles[i][2] - ClippedTriangles[i][3]).Normalised();
	}

	for (const auto& face : ResultFaces)
	{
		
	}
}

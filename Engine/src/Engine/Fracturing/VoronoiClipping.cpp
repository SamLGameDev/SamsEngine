#include "VoronoiClipping.h"

void VoronoiClipping::ClipMeshToVoronoi(const Voronoi& Diagram, const Model& Mesh)
{
	for (const auto& fracture : Diagram.Fractures)
	{
		for (const auto& face : fracture.CellFaces)
		{
			for (const auto& mesh : Mesh.Meshes)
			{
				for ()
			}
		}
	}
}

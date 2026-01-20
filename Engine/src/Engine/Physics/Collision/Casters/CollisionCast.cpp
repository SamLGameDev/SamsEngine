// DO NOT MARK.
//This is because it has been submitted for my dissertation. Link to Original: https://github.falmouth.ac.uk/GA-Undergrad-Student-Work-25-26/Dissertation-SL295211.git


#include "CollisionCast.h"
#include "Model.h"
#include "Voronoi.h"
#include "Math/MathCore.h"

Array<RayCastHit> CollisionCast::RayCastMeshAll(const Vector3D& Start, const Vector3D& Dir, const Model* InModel)
{
	const Vector3D colA = -Dir;

	Array<RayCastHit> hits;

	for (const Mesh& mesh : InModel->Meshes)
	{
		for (unsigned int Triangle = 0; Triangle < mesh.Indices.GetSize(); Triangle += 3)
		{
			const Vector3D colB = mesh.Vertices[Triangle + 1].Position - mesh.Vertices[Triangle].Position;

			const Vector3D colC = mesh.Vertices[Triangle + 2].Position - mesh.Vertices[Triangle].Position;

			const Vector3D colD = Start - mesh.Vertices[Triangle].Position;

			const float d0 = MathCore::Determinant(colA, colB, colC);

			if (MathCore::IsNearlyZero(d0))
			{
				continue;
			}

			const float dt = MathCore::Determinant(colD, colB, colC);

			float tStar = dt / d0;

			if (tStar < 0)
			{
				continue;
			}

			const float du = MathCore::Determinant(colA, colD, colC);

			const float uStar = du / d0;

			if (uStar < 0 || uStar > 1)
			{
				continue;
			}

			const float dv = MathCore::Determinant(colA, colB, colD);

			const float vStar = dv / d0;
			if (vStar < 0 || vStar > 1 - uStar)
			{
				continue;
			}

			const auto Hit = RayCastHit(Start + Dir * tStar);
			hits.Add(Hit);
		}
	}

	return hits;
}

bool CollisionCast::RayCastShape(const Vector3D& Start, const Vector3D& Dir, const Array<Vector3D>& Verts, const Array<uint16_t>& Inds)
{
	const Vector3D colA = -Dir;

	uint8_t numHits = 0;

	for (unsigned int Triangle = 0; Triangle < Inds.GetSize(); Triangle += 3)
		{
			const Vector3D colB = Verts[Triangle + 1] - Verts[Triangle];

			const Vector3D colC = Verts[Triangle + 2] - Verts[Triangle];

			const Vector3D colD = Start - Verts[Triangle];

			const float d0 = MathCore::Determinant(colA, colB, colC);

			if (MathCore::IsNearlyZero(d0))
			{
				continue;
			}

			const float dt = MathCore::Determinant(colD, colB, colC);

			float tStar = dt / d0;

			if (tStar < 0)
			{
				continue;
			}

			const float du = MathCore::Determinant(colA, colD, colC);

			const float uStar = du / d0;

			if (uStar < 0 || uStar > 1)
			{
				continue;
			}

			const float dv = MathCore::Determinant(colA, colB, colD);

			const float vStar = dv / d0;
			if (vStar < 0 || vStar > 1 - uStar)
			{
				continue;
			}
			numHits++;
		}

	if (numHits % 2 == 1)
	{
		return true;
	}
	return false;
}

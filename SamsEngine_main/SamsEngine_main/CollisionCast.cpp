#include "CollisionCast.h"
#include "Model.h"
#include "Math.h"

CollisionCast::CollisionCast()
{
}

Array<RayCastHit> CollisionCast::RayCastMeshAll(Vector3D Start, Vector3D Dir, Model* InModel)
{

	Vector3D colA = -Dir;

	Array<RayCastHit> Hits;

	for (unsigned int i  = 0; i < InModel->Meshes.GetSize(); i++)
	{
		for (unsigned int Triangle = 0; Triangle < InModel->Meshes[i].Indices.GetSize(); Triangle+=3)
		{
			Vector3D colB = InModel->Meshes[i].Vertices[Triangle + 1].Position - InModel->Meshes[i].Vertices[Triangle].Position;

			Vector3D colC = InModel->Meshes[i].Vertices[Triangle + 2].Position - InModel->Meshes[i].Vertices[Triangle].Position;

			Vector3D colD = Start - InModel->Meshes[i].Vertices[Triangle].Position;

			float d0 = Math::Determinant(colA, colB, colC);

			if (Math::IsNearlyZero(d0))
			{
				continue;
			}

			float dt = Math::Determinant(colD, colB, colC);

			float tStar = dt / d0;

			if (tStar < 0)
			{
				continue;
			}

			float du = Math::Determinant(colA, colD, colC);

			float uStar = du / d0;

			if (uStar < 0 || uStar > 1)
			{
				continue;
			}

			float dv = Math::Determinant(colA, colB, colD);

			float vStar = dv / d0;

			if(vStar < 0 || vStar > 1-uStar)
			{
				continue;
			}

			RayCastHit Hit;
			Hit.HitPoint = Start + Dir * tStar;
			Hits.Add(Hit);
	
		}
	}


	return Hits;
}

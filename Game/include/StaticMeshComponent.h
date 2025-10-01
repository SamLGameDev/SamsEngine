#pragma once
#include "Model.h"
#include "Transform.h"
#include "WorldObject.h"

class WorldObject;

class StaticMeshComponent
{
public:

	void SetMesh(Model* InMesh)
	{
		Mesh = InMesh;
		Mesh->AddInstance(&transform.Parent->transform);
	}

	Transform transform;
protected:

	Model* Mesh;
};

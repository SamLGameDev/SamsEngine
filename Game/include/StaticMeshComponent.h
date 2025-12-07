// DO NOT MARK.
//This is because it has been submitted for my dissertation. Link to Original: https://github.falmouth.ac.uk/GA-Undergrad-Student-Work-25-26/Dissertation-SL295211.git


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
		//Mesh->AddInstance(&transform.Parent->transform);
	}

	Transform transform;
protected:

	Model* Mesh;
};

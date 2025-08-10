#pragma once

#include "Array.h"
#include "Vector3D.h"

struct Vector3D;
class Model;

struct RayCastHit
{
public:
	Vector3D HitPoint;
};

class CollisionCast
{

public:

	CollisionCast();

	static Array<RayCastHit> RayCastMeshAll(Vector3D Start, Vector3D Dir, Model* InModel);



};


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

	CollisionCast() = default;


	/**
	 * How many times odes a ray hit the mesh
	 * @param Start The start position of the ray
	 * @param Dir Direction the ray is pointing
	 * @param InModel The model to compare against
	 * @return An Array of RayCast Hits
	 */
	[[nodiscard]] static Array<RayCastHit> RayCastMeshAll(const Vector3D& Start, const Vector3D& Dir, const Model* InModel);
};

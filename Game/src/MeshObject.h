#pragma once
#include <WorldObject.h>
#include <Transform.h>
#include "StaticMeshComponent.h"

/**
 * A basic class that just contains a static mesh, used to place stuff in the world
 */
class MeshObject : public WorldObject
{
public:

	MeshObject();


	/**
	 * The static mesh manager, set the mesh through this
	 */
	StaticMeshComponent SMesh;

private:
};

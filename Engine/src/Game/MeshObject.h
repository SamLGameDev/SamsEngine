// DO NOT MARK.
//This is because it has been submitted for my dissertation. Link to Original: https://github.falmouth.ac.uk/GA-Undergrad-Student-Work-25-26/Dissertation-SL295211.git


#pragma once
#include <WorldObject.h>
#include <Transform.h>
#include "Components/StaticMeshComponent.h"

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

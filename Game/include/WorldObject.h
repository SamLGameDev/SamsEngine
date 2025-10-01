#pragma once

#include "World.h"
#include "Transform.h"


/**
 * The base class all placed world objects should inherit from
 */
class WorldObject : public Object
{
public:

	WorldObject();

	//TODO make this a func
	/**
	 * A pointer to the game world
	 */
	static World* World;

	/**
	 * used for world location, change this to move/rotate/scale objects
	 */
	Transform transform;
};

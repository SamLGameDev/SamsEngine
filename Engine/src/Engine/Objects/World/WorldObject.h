// DO NOT MARK 
//This is because it has been submitted for my COMP305. Link to Original: https://github.falmouth.ac.uk/GA-Undergrad-Student-Work-25-26/Comp305-Engine-SL295211.git
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

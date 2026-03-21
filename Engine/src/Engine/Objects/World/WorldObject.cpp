// DO NOT MARK 
//This is because it has been submitted for my COMP305. Link to Original: https://github.falmouth.ac.uk/GA-Undergrad-Student-Work-25-26/Comp305-Engine-SL295211.git
#include "WorldObject.h"

World* WorldObject::World;

WorldObject::WorldObject() : Object()
{
	transform.Position = Vector3D(0, 0, 0);
	transform.Rotation = Vector3D(0, 0, 0);
	transform.Scale = Vector3D(1, 1, 1);
}

#include "WorldObject.h"

World* WorldObject::World;

WorldObject::WorldObject() : Object()
{
	transform.Position = Vector3D(0, 0, 0);
	transform.Rotation = Vector3D(0, 0, 0);
	transform.Scale = Vector3D(1, 1, 1);
}
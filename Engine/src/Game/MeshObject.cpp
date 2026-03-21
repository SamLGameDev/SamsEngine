

#include "MeshObject.h"
MeshObject::MeshObject() : WorldObject()
{
	SMesh.transform.Parent = this;
}

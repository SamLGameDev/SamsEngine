#include "Object.h"

MulticastDelegate<const double&> Object::TickDel;

Object::~Object()
{
	TickDel.Remove(this, &Object::Tick);
}

void Object::Tick(const double& DeltaTime)
{
}

void Object::Start()
{
	TickDel.BindMember(this, &Object::Tick);
}

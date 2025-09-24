#include "Object.h"

MulticastDelegate<const float&> Object::TickDel;

void Object::Tick(const float& DeltaTime)
{
}

void Object::Start()
{
	TickDel.BindMember(this, &Object::Tick);
}
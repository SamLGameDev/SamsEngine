#include "Object.h"

MulticastDelegate<const double&> Object::TickDel;

void Object::Tick(const double& DeltaTime)
{
}

void Object::Start()
{
	TickDel.BindMember(this, &Object::Tick);
}

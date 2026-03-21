// DO NOT MARK 
//This is because it has been submitted for my COMP305. Link to Original: https://github.falmouth.ac.uk/GA-Undergrad-Student-Work-25-26/Comp305-Engine-SL295211.git
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

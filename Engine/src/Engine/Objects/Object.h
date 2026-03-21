// DO NOT MARK 
//This is because it has been submitted for my COMP305. Link to Original: https://github.falmouth.ac.uk/GA-Undergrad-Student-Work-25-26/Comp305-Engine-SL295211.git
#pragma once

#include "BaseDelegate.h"
/**
 * The Base class of the engine, anything that needs a tick or start should inherit from this
 */
class Object
{
public:
	virtual ~Object();


	/**
	 * Called every frame
	 * @param DeltaTime The time since last frame
	 */
	virtual void Tick(const double& DeltaTime);

	/**
	 * All objects tick functions, the main game loop broadcasts this
	 */
	static MulticastDelegate<const double&> TickDel;


	/**
	 * Called on object creation, only once. Child classes should Implement the base version
	 */
	virtual void Start();

};

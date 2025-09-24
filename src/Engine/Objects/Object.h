#pragma once

#include "DataTypes/BaseDelegate.h"
/**
 * The Base class of the engine, anything that needs a tick or start should inherit from this
 */
class Object
{
public:
	virtual ~Object() = default;


	/**
	 * Called every frame
	 * @param DeltaTime The time since last frame
	 */
	virtual void Tick(const float& DeltaTime);

	/**
	 * All objects tick functions, the main game loop broadcasts this
	 */
	static MulticastDelegate<const float&> TickDel;


	/**
	 * Called on object creation, only once. Child classes should Implement the base version
	 */
	virtual void Start();

};

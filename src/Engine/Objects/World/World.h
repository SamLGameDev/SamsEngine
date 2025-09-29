#pragma once
#include "Object.h"


/**
 * The main world the objects are in.
 * TODO Move main game loop here, i think?
 */
class World final : Object
{
public:
	World() = default;

	void Tick(const double& InDeltaTime) override;

	void Start() override;


	/**
	 * @return The current delta time, i.e. the time since the last frame
	 */
	float GetDeltaTime() const
	{
		return DeltaTime;
	}

private:


	/**
	 * A pointer the the global time buffer for shaders, stores the time elapsed since program start \n
	 * Buffer location = 1
	 */
	double* TimeBufferPtr;


	/**
	 * The time since the last frame, will be updated in tick
	 */
	float DeltaTime;
};

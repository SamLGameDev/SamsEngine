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
	 * The time since the last frame, will be updated in tick
	 */
	float DeltaTime;
};

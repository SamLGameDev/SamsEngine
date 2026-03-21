// DO NOT MARK 
//This is because it has been submitted for my COMP305. Link to Original: https://github.falmouth.ac.uk/GA-Undergrad-Student-Work-25-26/Comp305-Engine-SL295211.git
#pragma once

#include "Vector3D.h"
#include <glm/glm.hpp>

class WorldObject;


struct GlobalTransforms
{
	glm::mat4 View;
	glm::mat4 Projection;
};

struct PerInstanceTransforms
{
	glm::mat4 Model;
	Vector3D Color;
};

/**
 * Main class that dictates an objects position
 */
class Transform
{
public:

	Transform() = default;

	Transform(const Vector3D& InPosition, const Vector3D& InScale, const Vector3D& InRotation);

	/**
	 * Calculates the bounds of the object based on its position, rotation and scale
	 * Bounds are the center, and 4 corners of the object to make a cube
	 */
	void CalculateBounds();

	//TODO make helper functions for setting position, rotation and scale


	/**
	 * Location of the object
	 */
	Vector3D Position = { 0, 0, 0 };


	/**
	 * Scale of the object
	 */
	Vector3D Scale {1, 1, 1};


	/**
	 * Rotation of the object
	 */
	Vector3D Rotation {0, 0, 0};


	/**
	 * Width, Height, and length of an object
	 */
	Vector3D Bounds;


	/**
	 * Half width, height, an length of an object
	 */
	Vector3D HalfBounds;


	/**
	 * Untranslated center of an object
	 */
	Vector3D Center;


	/**
	 * Translated center of an object
	 */
	Vector3D TransCenter;


	/**
	 * Translated bounds of an object
	 */
	Vector3D TransBounds;


	/**
	 * Translated half bounds of an object
	 */
	Vector3D TransHalfBounds;


	/**
	 * Generate a random point in the bounds of an object. \n NOTE: not mesh, cube bounds
	 */
	[[nodiscard]] Vector3D GetRandomPointInBounds() const;

	//TODO make it so these arent reliant in being set in the model
	Vector3D HighestPoints = Vector3D::NumericMin;
	Vector3D LowestPoints = Vector3D::NumericMax;

	[[nodiscard]] glm::mat4 GetModelMatrix() const;

	[[nodiscard]] glm::mat4 GetInverseModelMatrix() const;


	//TODO make this actually take the parents transform into account
	/**
	 * The object that this object is relative to.
	 */
	WorldObject* Parent;
};

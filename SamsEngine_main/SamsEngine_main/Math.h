#pragma once

#include "Vector3D.h"
#include <iostream>

class Math
{
public:

	static float Determinant(Vector3D C0, Vector3D C1, Vector3D C2);

	static bool IsNearlyZero(float Value, float range = 1e-6f)
	{
		return std::abs(Value) < range;
	}

	static bool IsEven(int Value);

};


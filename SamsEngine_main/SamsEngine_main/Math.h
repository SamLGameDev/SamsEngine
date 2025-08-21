#pragma once

#include <iostream>

struct Vector3D;

class Math
{
public:

	static float Determinant(Vector3D C0, Vector3D C1, Vector3D C2);

	static bool IsNearlyZero(float Value, float range = 1e-6f)
	{
		return std::abs(Value) < range;
	}

	static bool IsNearlyEqual(float a, float b, float range = 1e-6f);

	static bool IsEven(int Value);

};


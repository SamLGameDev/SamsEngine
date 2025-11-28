#pragma once

#include <cmath>

struct Vector3D;

class MathCore
{
public:

	[[nodiscard]] static float Determinant(const Vector3D& C0, const Vector3D& C1, const Vector3D& C2);

	[[nodiscard]] static bool IsNearlyZero(const float Value, const float range = 1e-6f)
	{
		return std::fabs(Value) < range;
	}

	[[nodiscard]] static bool IsNearlyEqual(float a, float b, float range = 1e-6f);

	[[nodiscard]] static bool IsEven(int Value);
	template <typename T>
	static T RandomRange(const T& min, const T& max);
};

template <typename T>
T MathCore::RandomRange(const T& min, const T& max)
{
	std::random_device rd;
	std::mt19937 gen(rd());

	const T  minX = std::min(min, max);
	const T maxX = std::max(min, max);

	std::uniform_int_distribution<T> distrX(minX, maxX);

	auto point = distrX(gen);

	return point;
}

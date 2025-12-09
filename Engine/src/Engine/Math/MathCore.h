#pragma once

#include <cmath>
#include <random>

struct Vector3D;

class MathCore
{
public:

	[[nodiscard]] static float Determinant(const Vector3D& C0, const Vector3D& C1, const Vector3D& C2);

	[[nodiscard]] static bool IsNearlyZero(const float Value, const float Range = 1e-6f)
	{
		return std::fabs(Value) < Range;
	}

	[[nodiscard]] static bool IsNearlyEqual(const float& A, const float& B, const float& Range = 1e-6f);

	[[nodiscard]] static bool IsEven(const int& Value);
	template <typename T>
	static T RandomRange(const T& Min, const T& Max);
};

template <typename T>
T MathCore::RandomRange(const T& Min, const T& Max)
{
	std::random_device rd;
	std::mt19937 gen(rd());

	const T  minX = std::min(Min, Max);
	const T maxX = std::max(Min, Max);

	std::uniform_int_distribution<T> distrX(minX, maxX);

	auto point = distrX(gen);

	return point;
}

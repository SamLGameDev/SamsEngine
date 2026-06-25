// DO NOT MARK 
//This is because it has been submitted for my COMP305. Link to Original: https://github.falmouth.ac.uk/GA-Undergrad-Student-Work-25-26/Comp305-Engine-SL295211.git
#pragma once

#include <cmath>
#include <random>

struct Vector3D;

class MathCore
{
public:

	[[nodiscard]] static float Determinant(const Vector3D& C0, const Vector3D& C1, const Vector3D& C2);

	[[nodiscard]] static bool IsNearlyZero(const double& Value, const double& Range = 1e-6f)
	{
		return Value < Range && Value > -Range;
	}

	[[nodiscard]] static bool IsNearlyEqual(const double& A, const double& B, const double& Range = 1e-6f);


	template<typename T> requires std::is_arithmetic_v<T>
	[[nodiscard]] static constexpr T Wrap(const T Min, const T Max, const T Num);

	/// <summary>
	/// Will never return the Max value
	/// </summary>
	/// <typeparam name="T"></typeparam>
	/// <param name="Min"></param>
	/// <param name="Max"></param>
	/// <param name="Num"></param>
	/// <returns></returns>
	template<typename T> requires std::is_arithmetic_v<T>
	[[nodiscard]] static constexpr T WrapExclusive(const T Min, const T Max, const T Num);

	template<typename T> requires std::is_arithmetic_v<T>
	[[nodiscard]] static constexpr bool WithinRange(const T Min, const T Max, const T Num);

	[[nodiscard]] static bool IsEven(const int& Value);
	template <typename T>
	static T RandomRange(const T& Min, const T& Max);
};

template <typename T> requires std::is_arithmetic_v<T>
constexpr T MathCore::Wrap(const T Min, const T Max, const T Num)
{


	const T size = Max - Min;

	if (size == 0) return Min;


	if (Num < Min)
	{
		const T mod = (Min - Num) % size;
		return (mod != static_cast<T>(0)) ? Max - mod : Min;
	}

	if (Num > Max)
	{
		const T mod = (Num - Max) % size;
		return (mod != static_cast<T>(0)) ? Min + mod : Max;
	}

	return Num;

}

template <typename T> requires std::is_arithmetic_v<T>
constexpr T MathCore::WrapExclusive(const T Min, const T Max, const T Num)
{
	const T size = Max - Min;

	if (size == 0) return Min;

	T Mod;
	if (Num < Min)
	{
		Mod = (Min - Num) % size;
		if (Mod > 0)
		{
			Mod = size - Mod;
		}
	}
	else
	{
		Mod = (Num - Min) % size;
	}

	return (T)(Min + Mod);
}

template <typename T> requires std::is_arithmetic_v<T>
constexpr bool MathCore::WithinRange(const T Min, const T Max, const T Num)
{
	if (Min > Max) WithinRange(Max, Min, Num);

	return Num >= Min && Num <= Max;
}

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

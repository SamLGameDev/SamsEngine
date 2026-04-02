// DO NOT MARK
//This is because it has been submitted for my COMP305. Link to Original: https://github.falmouth.ac.uk/GA-Undergrad-Student-Work-25-26/Comp305-Engine-SL295211.git
#pragma once

#include <cmath>
#include <cstdint>



struct Vector3D;

struct Vector2D
{
public:

	float X, Y;

	constexpr Vector2D()noexcept : X(0), Y(0) {};

	constexpr Vector2D(const float InX, const float InY)noexcept : X(InX), Y(InY) {};
	constexpr Vector2D(const double InX, const double InY)noexcept : X(static_cast<float>(InX)), Y(static_cast<float>(InY)) {}
	constexpr Vector2D(const size_t InX, const size_t InY)noexcept : X(static_cast<float>(InX)), Y(static_cast<float>(InY)) {}
	constexpr Vector2D(const size_t InX, const int InY)noexcept : X(static_cast<float>(InX)), Y(static_cast<float>(InY)) {}
	Vector2D(const Vector3D& Other);


	constexpr Vector2D(const std::uint32_t InX, const std::uint32_t InY)noexcept : X(static_cast<float>(InX)), Y(static_cast<float>(InY)) {}

	constexpr Vector2D(const int InX, const int InY)noexcept : X(static_cast<float>(InX)), Y(static_cast<float>(InY)) {}

	[[nodiscard]] constexpr bool operator==(const Vector2D& other) const {
		return IsAlmostEqual(*this, other);
	}

	[[nodiscard]] Vector2D operator+(const Vector2D& other)const
	{
		return { X + other.X, Y + other.Y };
	}

	[[nodiscard]] Vector2D operator/(const float& other)const
	{
		return { X / other, Y / other};
	}
	[[nodiscard]] Vector2D operator/(const Vector2D& other)const
	{
		return { X / other.X, Y / other.Y};
	}
	[[nodiscard]] Vector2D operator-() const {
		return { -X, -Y};
	}
	[[nodiscard]] constexpr Vector2D operator-(const Vector2D& other) const {
		return { X - other.X, Y - other.Y};
	}
	[[nodiscard]] Vector2D operator*(const float& multiplier)const
	{
		return { X * multiplier, Y * multiplier};
	}

	[[nodiscard]] constexpr bool operator<=(const double& Value) const
	{
		return X <= Value && Y <= Value;
	}

	static Vector2D RandomRange(const Vector2D& Min, const Vector2D& Max);
	static void Clamp(Vector2D& Value, const Vector2D& MinRange, const Vector2D& MaxRange);

	static Vector2D PerpendicularBisector(const Vector2D& P1, const Vector2D& P2, float& A, float& B, float& C);
	static float Dot(const Vector2D& A, const Vector2D& B);
	Vector2D Normalised() const;
	float GetLength() const;
	float GetSquaredLength() const;
	void Print() const;
	static float Cross(const Vector2D& a, const Vector2D& b);

	static Vector2D Lerp(const Vector2D& A, const Vector2D& B, const float& T);

	const static Vector2D Zero;
	[[nodiscard]] static constexpr bool IsAlmostEqual(const Vector2D& A, const Vector2D& B, const float& Range = 1e-6f)
	{
		Vector2D diff = A - B;
		return diff.Abs() <= Range;
	}
	[[nodiscard]] static constexpr Vector2D Abs(const Vector2D& V)
	{
		return { std::fabs(V.X), std::fabs(V.Y) };
	}
	[[nodiscard]] constexpr Vector2D Abs() const
	{
		return Abs(*this);
	}

};


[[nodiscard]] inline Vector2D operator*(const float& multiplier, const Vector2D& vec)
{
	return vec * multiplier;
}

#pragma once
struct Vector2D
{
public:

	float X, Y;

	constexpr Vector2D()noexcept : X(0), Y(0) {};

	constexpr Vector2D(const float InX, const float InY)noexcept : X(InX), Y(InY) {};

	[[nodiscard]] constexpr bool operator==(const Vector2D& other) const {
		return	X == other.X && Y == other.Y;
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
	[[nodiscard]] Vector2D operator-(const Vector2D& other) const {
		return { X - other.X, Y - other.Y};
	}
	[[nodiscard]] Vector2D operator*(const float& multiplier)const
	{
		return { X * multiplier, Y * multiplier};
	}

	~Vector2D() = default;
	static Vector2D RandomRange(const Vector2D& min, const Vector2D& max);
	static void Clamp(Vector2D& Value, const Vector2D& MinRange, const Vector2D& MaxRange);

	static Vector2D PerpendicularBisector(const Vector2D& P1, const Vector2D& P2, float& a, float& b, float& c);
	static float Dot(const Vector2D& a, const Vector2D& b);
	Vector2D Normalised() const;
	float GetLength() const;
	float GetSquaredLength() const;
	void Print() const;
	static float Cross(const Vector2D& a, const Vector2D& b);

	const static Vector2D Zero;
};
[[nodiscard]] inline Vector2D operator*(const float& multiplier, const Vector2D& vec)
{
	return vec * multiplier;
}

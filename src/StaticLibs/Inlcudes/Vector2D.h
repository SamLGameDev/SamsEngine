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

	~Vector2D() = default;

	const static Vector2D Zero;
};

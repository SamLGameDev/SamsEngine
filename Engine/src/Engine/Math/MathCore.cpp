// DO NOT MARK 
//This is because it has been submitted for my COMP305. Link to Original: https://github.falmouth.ac.uk/GA-Undergrad-Student-Work-25-26/Comp305-Engine-SL295211.git
#include "MathCore.h"

#include <random>

#include "Vector3D.h"

float MathCore::Determinant(const Vector3D& C0, const Vector3D& C1, const Vector3D& C2)
{
	const float d00 = C0.X * (C1.Y * C2.Z - C1.Z * C2.Y);

	const float d01 = C1.X * (C0.Y * C2.Z - C0.Z * C2.Y);

	const float d02 = C2.X * (C0.Y * C1.Z - C0.Z * C1.Y);

	return d00 - d01 + d02;
}

bool MathCore::IsNearlyEqual(const float& A, const float& B, const float& Range)
{
	return std::abs(A - B) < Range;
}

bool MathCore::IsEven(const int& Value)
{
	return Value % 2 == 0;
}

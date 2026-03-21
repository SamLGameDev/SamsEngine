// DO NOT MARK
//This is because it has been submitted for my COMP305. Link to Original: https://github.falmouth.ac.uk/GA-Undergrad-Student-Work-25-26/Comp305-Engine-SL295211.git
#pragma once
#include <cmath>


//Needs refactoring to allow different rows and columns, but for now this will do.

template <size_t N>
struct Row
{
public:
	float RowValues[N];
};

template <size_t RowNum, size_t Collum>
struct Matrix
{
public:

	Row<Collum> Rows[RowNum];

	float CalculateDeterminant();
};

/// <summary>
/// Will not return correct result for singular matrices, or if row and collum are not equal.
/// </summary>
/// <typeparam name="Row"></typeparam>
/// <typeparam name="Collum"></typeparam>
/// <returns></returns>
template <size_t Row, size_t Collum>
float Matrix<Row, Collum>::CalculateDeterminant()
{
	float determinate = 1.f;
	std::int8_t sign = 1;

	for (size_t i = 0; i < Row; i++)
	{
		float maxVal = std::fabs(Rows[i].RowValues[i]);
		size_t pivot = i;

		for (size_t j = i + 1; j < Row; j++)
		{
			if (std::fabs(Rows[j].RowValues[i]) > maxVal)
			{
				maxVal = std::fabs(Rows[j].RowValues[i]);
				pivot = j;
			}
		}
		
		if (pivot != i)
		{
			std::swap(Rows[i], Rows[pivot]);
			sign = -sign;
		}

		if (std::fabs(Rows[i].RowValues[i]) < 1e-6)
		{
			return 0.0f;
		}

		for (size_t j = i + 1; j < Row; j++)
		{
			const float factor = Rows[j].RowValues[i] / Rows[i].RowValues[i];
			for (size_t k = i + 1; k < Collum; k++)
			{
				Rows[j].RowValues[k] -= factor * Rows[i].RowValues[k];
			}
		}

	}

	for (size_t i = 0; i < Row; i++)
	{
		determinate *= Rows[i].RowValues[i];
	}

	return determinate * sign;

}

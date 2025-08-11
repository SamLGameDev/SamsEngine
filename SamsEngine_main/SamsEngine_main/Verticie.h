#pragma once
#include "Vector3D.h"
#include "Vector2D.h"
#include <iostream>
#include "Array.h"

struct Vector3D;



struct Vertex
{

public:
	

	Vector3D Position;

	Vector3D Normal;

	Vector2D TexCoords;

	Vector3D Color;



	bool operator==(const Vertex& other) const {
		return Position == other.Position;
	}

};

namespace std {
	template <>
	struct hash<Vertex> {
		std::size_t operator()(const Vertex& v) const {
			std::size_t hx = std::hash<float>{}(v.Position.X);
			std::size_t hy = std::hash<float>{}(v.Position.Y);
			std::size_t hz = std::hash<float>{}(v.Position.Z);
			return hx ^ (hy << 1) ^ (hz << 2); // combine the hashes
		}
	};
}

struct Face
{
public:

	Array<Vertex> Verticies;
};


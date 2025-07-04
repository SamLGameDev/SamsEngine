#pragma once
#include "Vector3D.h"
#include <iostream>

class Vector3D;



class Vertex
{

public:
	
	Vertex() {};

	Vertex(const Vector3D InPosition);

	~Vertex();

	bool operator==(const Vertex& other) const {
		return Position == other.GetPosition();
	}

	inline Vector3D GetPosition() const
	{
		return Position;
	}

private:

	Vector3D Position;

};

namespace std {
	template <>
	struct hash<Vertex> {
		std::size_t operator()(const Vertex& v) const {
			std::size_t hx = std::hash<float>{}(v.GetPosition().X);
			std::size_t hy = std::hash<float>{}(v.GetPosition().Y);
			std::size_t hz = std::hash<float>{}(v.GetPosition().Z);
			return hx ^ (hy << 1) ^ (hz << 2); // combine the hashes
		}
	};
}


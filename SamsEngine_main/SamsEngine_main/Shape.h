#pragma once

#include "LinkedList.h"
#include "Verticie.h"
#include "Vector3D.h"
#include "Vector2D.h"
#include "Object.h"
#include "Shader.h"
#include "Texture.h"
#include "glm-1.0.1/glm/glm.hpp"
#include "glm-1.0.1/glm/gtc/matrix_transform.hpp"
#include "glm-1.0.1/glm/gtc/type_ptr.hpp"

class Shape : Object
{
public:

	Shape();

	virtual void Render() = 0;

	virtual int GetVerticiesSizeByte() = 0;

	virtual float* GetVertices() = 0;

	virtual void SetRenderOrder(LinkedList<int> Indices) = 0;

	virtual LinkedList<Vertex> GetVerts() = 0;

	virtual LinkedList<int> GetIndices() = 0;

	const virtual Vector3D GetColor() const
	{
		return Color;
	};

	const virtual LinkedList<Vector2D>& GetTextureCords()const
	{
		return TextureCords;
	};

	virtual void Draw(glm::mat4 Translation, glm::mat4 Model, glm::mat4 View, glm::mat4 Projection);

protected:

	Vector3D Color;

	LinkedList<Vector2D> TextureCords;

	Shader shader;
};


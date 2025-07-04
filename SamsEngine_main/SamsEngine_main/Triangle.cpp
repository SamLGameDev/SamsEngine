#include "Triangle.h"
#include "Verticie.h"
#include "Vector3D.h"
#include <glad/glad.h>
#include "GLFW/glfw3.h"
#include <iostream>
#include "stb_image.h"
#include <sstream>
#include <filesystem>
namespace fs = std::filesystem;

Triangle::Triangle(Vertex InPos1, Vertex InPos2, Vertex InPos3, Vector3D color, Vector2D TexPos1, Vector2D TexPos2, Vector2D TexPos3)
{
	stbi_set_flip_vertically_on_load(true);
	fs::path texPath = fs::absolute("Contents/Images/container.jpg");
	std::string texPathStr = texPath.string();  // Keep it alive
	const char* address = texPathStr.c_str();
	int width, height, nrChannels;
	unsigned char* data = stbi_load(address, &width, &height, &nrChannels, 0);
	if (!data)
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	unsigned int texture;
	glGenTextures(1, &texture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(data);
	Textures.Add(texture);

	fs::path texPath2 = fs::absolute("Contents/Images/awesomeface.png");
	std::string texPathStr2 = texPath2.string();  // Keep it alive
	const char* address2 = texPathStr2.c_str();
	int width2, height2, nrChannels2;
	unsigned char* data2 = stbi_load(address2, &width2, &height2, &nrChannels2, 0);
	if (!data2)
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	unsigned int texture2;
	glGenTextures(1, &texture2);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture2);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width2, height2, 0, GL_RGBA, GL_UNSIGNED_BYTE, data2);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(data2);

	Textures.Add(texture2);


	Verts.Add(InPos1);
	Verts.Add(InPos2);
	Verts.Add(InPos3);

	Vertices = new float[9];

	Vertices[0] = InPos1.GetPosition().X;
	Vertices[1] = InPos1.GetPosition().Y;
	Vertices[2] = InPos1.GetPosition().Z;
	Vertices[3] = InPos2.GetPosition().X;
	Vertices[4] = InPos2.GetPosition().Y;
	Vertices[5] = InPos2.GetPosition().Z;
	Vertices[6] = InPos3.GetPosition().X;
	Vertices[7] = InPos3.GetPosition().Y;
	Vertices[8] = InPos2.GetPosition().Z;

	TextureCords.Add(TexPos1);
	TextureCords.Add(TexPos2);
	TextureCords.Add(TexPos3);

	Color = color;
}

Triangle::~Triangle()
{
	delete[] Vertices;
}

void Triangle::Render()
{

}

void Triangle::SetRenderOrder(LinkedList<int> Indices)
{
	IndicesOrder = Indices;
//	IndicesOrder.Sort();
}

#pragma once

#include <iostream>
#include <string>
#include "Mesh.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class Model
{

public:

	Model(std::string Path, Shader InShader);

	void Draw();

	Shader* GetShader()
	{
		return &ModelShader;
	}

	Transform ModelTransform;

private:

	Array<Mesh> Meshes;

	std::string StorageLocation;

	std::string Directory;

	void LoadModel();

	void ProcessNode(aiNode* Node, const aiScene* Scene);

	Mesh ProcessMesh(aiMesh* InMesh, const aiScene* Scene);

	Array<Texture> LoadMaterialTextures(aiMaterial* Mat, aiTextureType Type, std::string TypeName);

	Shader ModelShader;

	static Array<Texture> LoadedTextures;



};


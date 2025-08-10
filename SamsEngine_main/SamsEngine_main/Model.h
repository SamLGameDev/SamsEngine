#pragma once

#include <iostream>
#include <string>
#include "Mesh.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class WireObject;

class Model
{

public:

	Model(std::string Path, Shader InShader);

	~Model();

	void Draw();

	Shader* GetShader()
	{
		return &ModelShader;
	}

	Transform ModelTransform;

	const bool IsPointInsideModel(Vector3D Point);

	Array<Mesh> Meshes;

	WireObject* BoundingBox;

private:



	std::string StorageLocation;

	std::string Directory;

	void LoadModel();

	void ProcessNode(aiNode* Node, const aiScene* Scene);

	Mesh ProcessMesh(aiMesh* InMesh, const aiScene* Scene);

	void LoadMaterialTextures(aiMaterial* Mat, aiTextureType Type, std::string TypeName);

	void CalculateBoundPoints(aiNode* Node, const aiScene* Scene);

	void CalculatePointsForMesh(aiMesh* InMesh);


	Shader ModelShader;

	static Array<Texture> LoadedTextures;

	float Time;

	int NumVerticies;



};


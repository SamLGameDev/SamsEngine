#pragma once


#include <string>
#include "Mesh.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <memory>
#include "WireShapes.h"



#include "Texture.h"

class WireObject;

class Model
{
public:


	/**
	 * @param Path The path to the model, i.e. Contents/Models/Model.obj
	 * @param InShader The shader for the model
	 */
	Model(const std::string& Path, const Shader& InShader);

	~Model();
	void Copy(const Model& Other);

	Model(const Model& Other);
	void Move(Model& Other);

	Model(Model&& Other) noexcept
	{
		Move(Other);
	}

	Model& operator=(const Model& Other)
	{
		if (this != &Other)
		{
			Copy(Other);
		}
		return *this;
	}
	Model& operator=(Model&& Other) noexcept
	{
		if (this != &Other)
		{
			Move(Other);
		}
		return *this;
	}

	/**
	 * Draws the model with the set shader
	 */
	void Draw();



	/**
	 * Draws the model with the input shader
	 */
	void Draw(Shader* InShader);




	////[[nodiscard]] Shader* GetShader()
	////{
	////	return &ModelShader;
	////}


	/**
	 * @return Returns true if the point is inside the model
	 */
	[[nodiscard]] bool IsPointInsideModel(const Vector3D& Point) const;

	Array<Mesh> Meshes;

	std::unique_ptr<WireObject> BoundingBox;


	//TODO get rid of this, need to rework bounds recalculation
	Transform ModelTransform;

private:

	std::string StorageLocation;


	/**
	 * The Directory the model is stored in
	 */
	std::string Directory;


	void LoadModel();

	void ProcessNode(const aiNode* Node, const aiScene* Scene, size_t& CurrentMesh);

	//TODO do something about this, even if it means creating our own file loader
	/**
	 * @return Converts aiMesh into our own mesh class
	 */
	[[nodiscard]] Mesh ProcessMesh(aiMesh* InMesh, const aiScene* Scene);

	void LoadMaterialTextures(const aiMaterial* Mat, const aiTextureType& Type, const TextureType& TypeName, Shader& MeshShader);


	/**
	 * Find the highest and lowest vertices for the width, height, and length of the Model
	 */
	void CalculateBoundPoints(const aiNode* Node, const aiScene* Scene);


	/**
	 * Find the highest and lowest vertices for the width, height, and length of the mesh
	 */
	void CalculatePointsForMesh(const aiMesh* InMesh);

	//Shader ModelShader;


	/**
	 * All the textures that have currently been loaded, so we don't have to load them again when we have duplicates
	 */
//	static Array<Texture> LoadedTextures;
#if DEBUG
	double Time;

	unsigned int NumVertices = 0;
#endif


};

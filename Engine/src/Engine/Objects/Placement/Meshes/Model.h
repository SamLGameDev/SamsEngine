#pragma once


#include <string>
#include "Mesh.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <memory>



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


	/**
	 * Draws the model with the set shader
	 */
	void Draw();


	/**
	 * Draws an outline of the model
	 */
	void DrawOutline();


	/**
	 * Draws the model with the input shader
	 */
	void Draw(Shader* InShader);




	[[nodiscard]] Shader* GetShader()
	{
		return &ModelShader;
	}


	/**
	 * @return Returns true if the point is inside the model
	 */
	[[nodiscard]] bool IsPointInsideModel(const Vector3D& Point) const;

	Array<Mesh> Meshes;

	std::unique_ptr<WireObject> BoundingBox;

	void SetOutlineShader(const Shader& Outline)
	{
		OutlineShader = Outline;
	}


	/**
	 * Adds a new instance to of the model, and updates the buffer
	 */
	void AddInstance(const Transform* transform);

	//TODO rework this to work per object, instead of mesh
	/**
	 * The Depth buffer draw group, should normally be GL_LESS.
	 */
	GLenum DrawGroup;

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

	Shader ModelShader;

	Shader OutlineShader;


	/**
	 * All the textures that have currently been loaded, so we don't have to load them again when we have duplicates
	 */
	static Array<Texture> LoadedTextures;
#if DEBUG
	double Time;

	unsigned int NumVertices = 0;
#endif

	constexpr static float OutlineSize = 0.1f;

	unsigned int Instances = 0;

	Array<const Transform*> InstanceTransforms;

	GLuint ModelVBO;


	/**
	 * The persistent uniform buffer ptr model transforms
	 */
	glm::mat4* modelTransforms;

	/**
 * Updates the stored model transformed to reflect the current positions of all instances of this model
 */
	void UpdateModelLocations() const;


};

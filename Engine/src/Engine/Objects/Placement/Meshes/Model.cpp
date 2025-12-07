#include "Model.h"
#include "WireShapes.h"
#include "CollisionCast.h"
#include "MathCore.h"
#include "UniformBufferFactory.h"
#include <algorithm>
#include <iostream>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include "CorePaths.h"
#include "InterfaceRenderer.h"

Array<Texture> Model::LoadedTextures;

Model::Model(const std::string& Path, const Shader& InShader)
{
#if DEBUG
	Time = glfwGetTime();

	std::cout << glfwGetTime() - Time << std::endl;
#endif

	StorageLocation =  CorePaths::Contents.Path + "/" + Path;

	ModelShader = InShader;

	Directory = Path.substr(0, Path.find_last_of('/'));

#if DEBUG
	std::cout << glfwGetTime() - Time << std::endl;
#endif

	ModelTransform = Transform(Vector3D(0, 0, 0), Vector3D(1, 1, 1), Vector3D(0, 0, 0));

	LoadModel();

#if DEBUG
	std::cout << "NumVertices: " << NumVertices << std::endl;
#endif

	BoundingBox = DrawWireCube({ 0, 0,0 }, ModelTransform.TransHalfBounds, Vector3D(1, 1, 1),
		Vector3D(0.2f, 0.5f, 0.2f));

	::Renderer::AddModel(this);
}

Model::~Model()
{
}

void Model::Copy(const Model& Other)
{
	Meshes = Other.Meshes;
	StorageLocation = Other.StorageLocation;
	Directory = Other.Directory;
	ModelTransform = Other.ModelTransform;
	ModelShader = Other.ModelShader;
	if (Other.BoundingBox)
	{
		BoundingBox = std::make_unique<WireObject>(*Other.BoundingBox);
	}
	else
	{
		BoundingBox = nullptr;
	}
}

Model::Model(const Model& Other)
{
	Copy(Other);
}

void Model::Move(Model& Other)
{
	Meshes = std::move(Other.Meshes);
	StorageLocation = std::move(Other.StorageLocation);
	Directory = std::move(Other.Directory);
	ModelTransform = Other.ModelTransform;
	ModelShader = std::move(Other.ModelShader);
	if (Other.BoundingBox)
	{
		BoundingBox = std::move(Other.BoundingBox);
	}
	else
	{
		BoundingBox = nullptr;
	}
}


void Model::Draw()
{

	for (Mesh& Mesh : Meshes)
	{
		Mesh.Draw(&ModelTransform);
	}
}

void Model::Draw(Shader InShader)
{

	for (Mesh& Mesh : Meshes)
	{
		Mesh.Draw(&ModelTransform, InShader);
	}
}

void Model::LoadModel()
{
	Assimp::Importer import;

	const aiScene* scene = import.ReadFile(StorageLocation, aiProcess_Triangulate | aiProcess_FlipUVs);

#if DEBUG
	std::cout << glfwGetTime() - Time << std::endl;
#endif

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{

#if DEBUG
		std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
#endif
		return;
	}
#if DEBUG
	std::cout << glfwGetTime() - Time << std::endl;
#endif

	CalculateBoundPoints(scene->mRootNode, scene);

	ModelTransform.CalculateBounds();

	size_t numMeshes = scene->mNumMeshes;
	Meshes.Reallocate(numMeshes);
	std::cout << "NumMeshes: " << numMeshes << std::endl;
	size_t currentMesh = 0;

	ProcessNode(scene->mRootNode, scene, currentMesh);
}

void Model::ProcessNode(const aiNode* Node, const aiScene* Scene, size_t& CurrentMesh)
{
#if DEBUG
	std::cout << glfwGetTime() - Time << std::endl;
#endif

	for (unsigned int i = 0; i < Node->mNumMeshes; i++)
	{
		aiMesh* mesh = Scene->mMeshes[Node->mMeshes[i]];
		Meshes[CurrentMesh] = ProcessMesh(mesh, Scene);
		CurrentMesh++;
		std::cout << "Processed Mesh " << CurrentMesh << std::endl;
	}

#if DEBUG
	std::cout << glfwGetTime() - Time << std::endl;
#endif

	for (unsigned int i = 0; i < Node->mNumChildren; i++)
	{
		ProcessNode(Node->mChildren[i], Scene, CurrentMesh);
	}
#if DEBUG
	std::cout << glfwGetTime() - Time << std::endl;
#endif
}

Mesh Model::ProcessMesh(aiMesh* InMesh, const aiScene* Scene)
{
	auto mesh = Mesh();

#if DEBUG
	double time = glfwGetTime();

	std::cout << "BeforeVertices: " << glfwGetTime() - time << std::endl;

	NumVertices += InMesh->mNumVertices;
#endif

	// saves processing time if we allocate beforehand, as we don't have to update each time
	mesh.Vertices.Reallocate(InMesh->mNumVertices);

	for (unsigned int i = 0; i < InMesh->mNumVertices; i++)
	{
		Vertex vert;

		vert.Position = Vector3D(InMesh->mVertices[i].x, InMesh->mVertices[i].y, InMesh->mVertices[i].z);

		if (InMesh->HasTextureCoords(0))
		{
			vert.TexCoords = Vector2D(InMesh->mTextureCoords[0][i].x, InMesh->mTextureCoords[0][i].y);
		}

		mesh.Vertices[i]=vert;
	}

#if DEBUG

	std::cout << "AfterVertices: " << glfwGetTime() - time << std::endl;

	time = glfwGetTime();
#endif

	for (unsigned int i = 0; i < InMesh->mNumFaces; i++)
	{
		for (unsigned int j = 0; j < InMesh->mFaces[i].mNumIndices; j++)
		{
			mesh.Indices.Add(InMesh->mFaces[i].mIndices[j]);
		}
	}
#if DEBUG
	std::cout << "AfterIndices: " << glfwGetTime() - time << std::endl;

	time = glfwGetTime();
#endif

	mesh.MeshShader = ModelShader;

	if (InMesh->mMaterialIndex != 0)
	{
		const aiMaterial* mat = Scene->mMaterials[InMesh->mMaterialIndex];

		LoadMaterialTextures(mat, aiTextureType_HEIGHT, height, mesh.MeshShader);

		LoadMaterialTextures(mat, aiTextureType_DIFFUSE, diffuse, mesh.MeshShader);

		//TODO: Extend this to cover more types, like base color

	}

#if DEBUG

	std::cout << "AfterTextures: " << glfwGetTime() - time << std::endl;

	time = glfwGetTime();
#endif

#if DEBUG

	std::cout << "AfterShader: " << glfwGetTime() - time << std::endl;

	time = glfwGetTime();

#endif

	mesh.Initialise();

#if DEBUG

	std::cout << "AfterGPUAllocation" << glfwGetTime() - time << std::endl;

#endif

	return mesh;
}

void Model::LoadMaterialTextures(const aiMaterial* Mat, const aiTextureType& Type, const TextureType& TypeName, Shader& MeshShader)
{
	for (unsigned int i = 0; i < Mat->GetTextureCount(Type); i++)
	{
		aiString str;
		Mat->GetTexture(Type, i, &str);

		const std::string Path = Directory + "\\" + str.C_Str();

		bool bSkip = false;

		for (unsigned int index = 0; index < LoadedTextures.GetSize(); index++)
		{
			const std::string texPath = LoadedTextures[index].GetTextureLocation();
			if (texPath == Path)
			{
				bSkip = true;
				break;
			}
		}

		if (bSkip)
		{
			continue;
		}

		const auto texture = Texture(Path, TypeName);

		LoadedTextures.Add(texture);
		MeshShader.AddTexture(texture);
	}
}

void Model::CalculateBoundPoints(const aiNode* Node, const aiScene* Scene)
{
	for (unsigned int i = 0; i < Node->mNumMeshes; i++)
	{
		CalculatePointsForMesh(Scene->mMeshes[Node->mMeshes[i]]);
	}

	for (unsigned int i = 0; i < Node->mNumChildren; i++)
	{
		CalculateBoundPoints(Node->mChildren[i], Scene);
	}
}

void Model::CalculatePointsForMesh(const aiMesh* InMesh)
{
	for (unsigned int i = 0; i < InMesh->mNumVertices; i++)
	{
		ModelTransform.HighestPoints.X = std::max(InMesh->mVertices[i].x, ModelTransform.HighestPoints.X);
		ModelTransform.LowestPoints.X = std::min(InMesh->mVertices[i].x, ModelTransform.LowestPoints.X);

		ModelTransform.HighestPoints.Y = std::max(InMesh->mVertices[i].y, ModelTransform.HighestPoints.Y);
		ModelTransform.LowestPoints.Y = std::min(InMesh->mVertices[i].y, ModelTransform.LowestPoints.Y);

		ModelTransform.HighestPoints.Z = std::max(InMesh->mVertices[i].z, ModelTransform.HighestPoints.Z);
		ModelTransform.LowestPoints.Z = std::min(InMesh->mVertices[i].z, ModelTransform.LowestPoints.Z);
	}
}

bool Model::IsPointInsideModel(const Vector3D& Point) const
{
	const Array<RayCastHit> Hits = CollisionCast::RayCastMeshAll(Point, Vector3D(1, 1, 1), this);

	//If even hits, it's outside, as it would have passed through the mesh twice, odd for inside

	if (MathCore::IsEven(static_cast<int>(Hits.GetSize())))
	{
		return false;
	}

	return true;
}

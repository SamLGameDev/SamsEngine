#include "Model.h"
#include "WireShapes.h"
#include "CollisionCast.h"
#include "MathCore.h"
#include "UniformBufferFactory.h"
#include <iostream>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include "CorePaths.h"

#include "Renderer.h"

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

	BoundingBox = DrawWireCube(ModelTransform.TransCenter, ModelTransform.TransHalfBounds, Vector3D(1, 1, 1),
		Vector3D(0.2f, 0.5f, 0.2f));

	Renderer::AddItemToRender(this);
}


Model::~Model()
{
}

void Model::Draw() const
{

	glDepthFunc(DrawGroup);
	UpdateModelLocations();

	for (const Mesh& Mesh : Meshes)
	{
		Mesh.Draw(&ModelTransform);
	}
}

void Model::DrawOutline()
{
	UpdateModelLocations();

	//Increase the scale so outline shows

	ModelTransform.Scale += OutlineSize;
	for (const Mesh& Mesh : Meshes)
	{
		Mesh.Draw(&ModelTransform, &OutlineShader);
	}
	ModelTransform.Scale -= OutlineSize;
}

void Model::Draw(const Shader* InShader) const
{
	glDepthFunc(DrawGroup);
	UpdateModelLocations();

	for (const Mesh& Mesh : Meshes)
	{
		Mesh.Draw(&ModelTransform, InShader);
	}
}

void Model::UpdateModelLocations() const
{
	for (unsigned int i = 0; i < InstanceTransforms.GetSize(); i++)
	{
		const glm::mat4 model = InstanceTransforms[i]->GetModelMatrix();
		memcpy(&modelTransforms[i], &model, sizeof(glm::mat4));
	}
}

void Model::AddInstance(const Transform* transform)
{
	//auto updates instances as all meshes have a reference to this
	Instances++;

	InstanceTransforms.Add(transform);

	constexpr GLenum flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;

	const GLsizeiptr size = sizeof(glm::mat4) * Instances;

	//delete the old buffer, as the new one will need to be bigger

	glDeleteBuffers(1, &ModelVBO);

	glGenBuffers(1, &ModelVBO);

	glBindBuffer(GL_ARRAY_BUFFER, ModelVBO);

	glBufferStorage(GL_ARRAY_BUFFER, size, nullptr, flags);

	modelTransforms = static_cast<glm::mat4*>(glMapBufferRange(GL_ARRAY_BUFFER, 0, size, flags));

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	for (Mesh& Mesh : Meshes)
	{

		glBindVertexArray(Mesh.VAO);

		//set the shaders vertex location. we have to do 4 here as it's a mat4, so essentially 4 vec4

		glBindBuffer(GL_ARRAY_BUFFER, ModelVBO);

		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), static_cast<void*>(nullptr));

		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), reinterpret_cast<void*>(sizeof(glm::vec4)));

		glEnableVertexAttribArray(5);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), reinterpret_cast<void*>(2 * sizeof(glm::vec4)));

		glEnableVertexAttribArray(6);
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), reinterpret_cast<void*>(3 * sizeof(glm::vec4)));

		glVertexAttribDivisor(3, 1);
		glVertexAttribDivisor(4, 1);
		glVertexAttribDivisor(5, 1);
		glVertexAttribDivisor(6, 1);

		Mesh.Instances = &Instances;
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

	//ProcessNode(scene->mRootNode, scene);
}

void Model::ProcessNode(const aiNode* Node, const aiScene* Scene)
{
#if DEBUG
	std::cout << glfwGetTime() - Time << std::endl;
#endif

	for (unsigned int i = 0; i < Node->mNumMeshes; i++)
	{
		aiMesh* mesh = Scene->mMeshes[Node->mMeshes[i]];
		Meshes.Add(ProcessMesh(mesh, Scene));
	}

#if DEBUG
	std::cout << glfwGetTime() - Time << std::endl;
#endif

	for (unsigned int i = 0; i < Node->mNumChildren; i++)
	{
		ProcessNode(Node->mChildren[i], Scene);
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

		vert.Normal = Vector3D(InMesh->mNormals[i].x, InMesh->mNormals[i].y, InMesh->mNormals[i].z);

		if (InMesh->HasTextureCoords(0))
		{
			vert.TexCoords = Vector2D(InMesh->mTextureCoords[0][i].x, InMesh->mTextureCoords[0][i].y);
		}

		mesh.Vertices.Add(vert);
	}

#if DEBUG

	std::cout << "AfterVertices: " << glfwGetTime() - time << std::endl;

	time = glfwGetTime();
#endif

	for (unsigned int i = 0; i < InMesh->mNumFaces; i++)
	{
		//aiFace face = InMesh->mFaces[i];
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

		LoadMaterialTextures(mat, aiTextureType_DIFFUSE, diffuse, mesh.MeshShader);

		LoadMaterialTextures(mat, aiTextureType_SPECULAR, specular, mesh.MeshShader);

		LoadMaterialTextures(mat, aiTextureType_HEIGHT, height, mesh.MeshShader);

		LoadMaterialTextures(mat, aiTextureType_AMBIENT, normal, mesh.MeshShader);

		LoadMaterialTextures(mat, aiTextureType_BASE_COLOR, diffuse, mesh.MeshShader);

		LoadMaterialTextures(mat, aiTextureType_GLTF_METALLIC_ROUGHNESS, specular, mesh.MeshShader);
		LoadMaterialTextures(mat, aiTextureType_NORMALS, normal, mesh.MeshShader);


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

	mesh.RegenerateMesh();

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
				MeshShader.AddTexture(LoadedTextures[index]);
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
		if (InMesh->mVertices[i].x > ModelTransform.HighestPoints.X)
		{
			ModelTransform.HighestPoints.X = InMesh->mVertices[i].x;
		}
		if (InMesh->mVertices[i].x < ModelTransform.LowestPoints.X)
		{
			ModelTransform.LowestPoints.X = InMesh->mVertices[i].x;
		}

		if (InMesh->mVertices[i].y > ModelTransform.HighestPoints.Y)
		{
			ModelTransform.HighestPoints.Y = InMesh->mVertices[i].y;
		}
		if (InMesh->mVertices[i].y < ModelTransform.LowestPoints.Y)
		{
			ModelTransform.LowestPoints.Y = InMesh->mVertices[i].y;
		}

		if (InMesh->mVertices[i].z > ModelTransform.HighestPoints.Z)
		{
			ModelTransform.HighestPoints.Z = InMesh->mVertices[i].z;
		}
		if (InMesh->mVertices[i].z < ModelTransform.LowestPoints.Z)
		{
			ModelTransform.LowestPoints.Z = InMesh->mVertices[i].z;
		}
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

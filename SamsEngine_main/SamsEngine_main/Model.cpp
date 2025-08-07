#include "Model.h"
#include "WireShapes.h"

Array<Texture> Model::LoadedTextures;

Model::Model(std::string Path, Shader InShader)
{
	Time = glfwGetTime();
	
	std::cout << glfwGetTime() - Time << std::endl;

	StorageLocation = Path;

	ModelShader = InShader;

	Directory = Path.substr(0, Path.find_last_of('/'));

	std::cout << glfwGetTime() - Time << std::endl;

	ModelTransform = Transform(Vector3D(0, 0, 0), Vector3D(1, 1, 1), Vector3D(0, 0, 0));

	LoadModel();

	std::cout << "NumVerticies: " << NumVerticies << std::endl;
}

Model::~Model()
{
}

void Model::Draw()
{
	for (unsigned int i = 0; i < Meshes.GetSize(); i++)
	{
		Meshes.GetItemAtRef(i)->Draw(&ModelTransform);
	}
}


void Model::LoadModel()
{
	Assimp::Importer import;

	const aiScene* scene = import.ReadFile(StorageLocation, aiProcess_Triangulate | aiProcess_FlipUVs);
	std::cout << glfwGetTime() - Time << std::endl;

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
		return;
	}
	std::cout << glfwGetTime() - Time << std::endl;
	//Meshes.Reallocate(scene->mNumMeshes);

	CalculateBoundPoints(scene->mRootNode, scene);

	ModelTransform.CalculateBounds();
	
	ProcessNode(scene->mRootNode, scene);

}

void Model::ProcessNode(aiNode* Node, const aiScene* Scene)
{
	std::cout << glfwGetTime() - Time << std::endl;
	for (unsigned int i = 0; i < Node->mNumMeshes; i++)
	{
		aiMesh* mesh = Scene->mMeshes[Node->mMeshes[i]];
		Meshes.Add(ProcessMesh(mesh, Scene));
	}
	std::cout << glfwGetTime() - Time << std::endl;

	for (unsigned int i = 0; i < Node->mNumChildren; i++)
	{
		ProcessNode(Node->mChildren[i], Scene);
	}
	std::cout << glfwGetTime() - Time << std::endl;

}

Mesh Model::ProcessMesh(aiMesh* InMesh, const aiScene* Scene)
{
	Mesh mesh = Mesh();

	float time = glfwGetTime();

	std::cout  << "BeforeVerticies: " << glfwGetTime() - time << std::endl;

	NumVerticies += InMesh->mNumVertices;

	mesh.Vertices.Reallocate(InMesh->mNumVertices);

	for (unsigned int i = 0; i < InMesh->mNumVertices; i++)
	{

		Vertex vert;

		//aiVector3D aiVert = InMesh->mVertices[i];

		//aiVector3D* aiNorms = InMesh->mNormals;

		vert.Position = Vector3D(InMesh->mVertices[i].x + ModelTransform.Center.X, InMesh->mVertices[i].y+ ModelTransform.Center.Y, InMesh->mVertices[i].z + ModelTransform.Center.Z);

		vert.Normal = Vector3D(InMesh->mNormals[i].x, InMesh->mNormals[i].y, InMesh->mNormals[i].z);

		if (InMesh->HasTextureCoords(0))
		{
			//aiVector3D* aiTexCoords = InMesh->mTextureCoords[0];
			vert.TexCoords = Vector2D(InMesh->mTextureCoords[0][i].x, InMesh->mTextureCoords[0][i].y);
		}
		else
		{
			vert.TexCoords = Vector2D(0, 0);
		}

		mesh.Vertices.Add(vert);
	}

	std::cout << "AfterVerticies: " << glfwGetTime() - time << std::endl;

	time = glfwGetTime();

	for (unsigned int i = 0; i < InMesh->mNumFaces; i++)
	{
		//aiFace face = InMesh->mFaces[i];
		for (unsigned int j = 0; j < InMesh->mFaces[i].mNumIndices; j++)
		{
			mesh.Indices.Add(InMesh->mFaces[i].mIndices[j]);
		}
	}


	std::cout << "AfterIndices: " << glfwGetTime() - time << std::endl;

	time = glfwGetTime();
	if (InMesh->mMaterialIndex >= 0)
	{
		aiMaterial* mat = Scene->mMaterials[InMesh->mMaterialIndex];

		LoadMaterialTextures(mat, aiTextureType_DIFFUSE, "texture_diffuse");

		LoadMaterialTextures(mat, aiTextureType_SPECULAR, "texture_specular");

		//textures.Add(diffuseMaps);
		//textures.Add(specularMaps);
		//ModelShader.AddTexture(diffuseMaps);
		//ModelShader.AddTexture(specularMaps);
	}


	std::cout << "AfterTextures: " << glfwGetTime() - time << std::endl;

	time = glfwGetTime();
	mesh.MeshShader = ModelShader;


	std::cout << "AfterShader: " << glfwGetTime() - time << std::endl;

	time = glfwGetTime();

	mesh.RegenerateMesh();


	std::cout << "AfterGPUAllocation" << glfwGetTime() - time << std::endl;

	return mesh;
}

void Model::LoadMaterialTextures(aiMaterial* Mat, aiTextureType Type, std::string TypeName)
{
	for (unsigned int i = 0; i < Mat->GetTextureCount(Type); i++)
	{
		aiString str;
		Mat->GetTexture(Type, i, &str);

		std::string Path = Directory + "\\" + str.C_Str();

		bool bSkip = false;

		for (unsigned int index = 0; index < LoadedTextures.GetSize(); index++)
		{
			std::string texPath = LoadedTextures[index].GetTextureLocation();
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

		Texture texture = Texture(Path, TypeName);

		LoadedTextures.Add(texture);
		ModelShader.AddTexture(texture);
	}
}

void Model::CalculateBoundPoints(aiNode* Node, const aiScene* Scene)
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

void Model::CalculatePointsForMesh(aiMesh* InMesh)
{
	for (unsigned int i = 0; i < InMesh->mNumVertices; i++)
	{

		if (InMesh->mVertices[i].x > ModelTransform.TopWidth)
		{
			ModelTransform.TopWidth = InMesh->mVertices[i].x;
		}
		if (InMesh->mVertices[i].x < ModelTransform.BottomWidth)
		{
			ModelTransform.BottomWidth = InMesh->mVertices[i].x;
		}

		if (InMesh->mVertices[i].y > ModelTransform.TopHeight)
		{
			ModelTransform.TopHeight = InMesh->mVertices[i].y;
		}
		if (InMesh->mVertices[i].y < ModelTransform.BottomHeight)
		{
			ModelTransform.BottomHeight = InMesh->mVertices[i].y;
		}

		if (InMesh->mVertices[i].z > ModelTransform.TopLength)
		{
			ModelTransform.TopLength = InMesh->mVertices[i].z;
		}
		if (InMesh->mVertices[i].z < ModelTransform.BottomLength)
		{
			ModelTransform.BottomLength = InMesh->mVertices[i].z;
		}
	}
}

#include "Model.h"

Array<Texture> Model::LoadedTextures;

Model::Model(std::string Path, Shader InShader)
{
	StorageLocation = Path;

	ModelShader = InShader;

	Directory = Path.substr(0, Path.find_last_of('/'));

	LoadModel();
}

void Model::Draw()
{
	for (unsigned int i = 0; i < Meshes.GetSize(); i++)
	{
		Meshes.GetItemAtRef(i)->Draw();
	}
}

void Model::LoadModel()
{
	Assimp::Importer import;

	const aiScene* scene = import.ReadFile(StorageLocation, aiProcess_Triangulate | aiProcess_FlipUVs);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
		return;
	}

	ProcessNode(scene->mRootNode, scene);
}

void Model::ProcessNode(aiNode* Node, const aiScene* Scene)
{
	for (unsigned int i = 0; i < Node->mNumMeshes; i++)
	{
		aiMesh* mesh = Scene->mMeshes[Node->mMeshes[i]];
		Meshes.Add(ProcessMesh(mesh, Scene));
	}

	for (unsigned int i = 0; i < Node->mNumChildren; i++)
	{
		ProcessNode(Node->mChildren[i], Scene);
	}

}

Mesh Model::ProcessMesh(aiMesh* InMesh, const aiScene* Scene)
{
	Array<Vertex> verticies;
	Array<unsigned int> indicies;
	Array<Texture> textures;

	for (unsigned int i = 0; i < InMesh->mNumVertices; i++)
	{
		Vertex vert;
		aiVector3D aiVert = InMesh->mVertices[i];

		aiVector3D* aiNorms= InMesh->mNormals;

		vert.Position = Vector3D(aiVert.x, aiVert.y, aiVert.z);
		vert.Normal = Vector3D(aiNorms->x, aiNorms->y, aiNorms->z);

		if (InMesh->HasTextureCoords(0))
		{
			aiVector3D* aiTexCoords = InMesh->mTextureCoords[0];
			vert.TexCoords = Vector2D(aiTexCoords[i].x, aiTexCoords[i].y);
		}
		else
		{
			vert.TexCoords = Vector2D(0, 0);
		}

		verticies.Add(vert);
	}

	for (unsigned int i = 0; i < InMesh->mNumFaces; i++)
	{
		aiFace face = InMesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
		{
			indicies.Add(face.mIndices[j]);
		}
	}

	if (InMesh->mMaterialIndex >= 0)
	{
		aiMaterial* mat = Scene->mMaterials[InMesh->mMaterialIndex];

		Array<Texture> diffuseMaps = LoadMaterialTextures(mat, aiTextureType_DIFFUSE, "texture_diffuse");

		Array<Texture> specularMaps = LoadMaterialTextures(mat, aiTextureType_SPECULAR, "texture_specular");

		textures.Add(diffuseMaps);
		textures.Add(specularMaps);
	}



	return Mesh(verticies, indicies, ModelShader);
}

Array<Texture> Model::LoadMaterialTextures(aiMaterial* Mat, aiTextureType Type, std::string TypeName)
{

	Array<Texture> textures;

	for (unsigned int i = 0; i < Mat->GetTextureCount(Type); i++)
	{
		aiString str;
		Mat->GetTexture(Type, i, &str);

		std::string Path = Directory + "\\" + str.C_Str();

		bool bSkip = false;

		for (unsigned int index = 0; index < LoadedTextures.GetSize(); i++)
		{
			std::string texPath = LoadedTextures[index].GetTextureLocation();
			if (texPath == Path)
			{
				textures.Add(LoadedTextures[i]);
				bSkip = true;
				break;
			}
		}

		if (bSkip)
		{
			continue;
		}

		Texture texture = Texture(Path, TypeName);
		textures.Add(texture);
	}

	return textures;
}

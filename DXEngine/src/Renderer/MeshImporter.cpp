#include "MeshImporter.h"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include "Mesh.h"

#include <string>

std::vector<SharedPtr<Mesh>> MeshImporter::ImportDynamicMesh(std::filesystem::path path)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path.string().c_str(), aiProcess_Triangulate | aiProcess_GenSmoothNormals |
		aiProcess_CalcTangentSpace);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "[ASSIMP] " << std::string(importer.GetErrorString()) << "\n";
		return std::vector<SharedPtr<Mesh>>();
	}

	std::vector<SharedPtr<Mesh>> retMeshes;

	for (uint32_t i = 0; i < scene->mNumMeshes; i++)
	{
		std::cout << "[ASSIMP] Importing mesh: " << scene->mMeshes[i]->mName.C_Str() << "\n";

		std::vector<MeshVertex> Vertices;
		std::vector<uint32_t> Indices;
		PBRMaterial material{};

		for (uint32_t j = 0; j < scene->mMeshes[i]->mNumVertices; j++)
		{
			MeshVertex vertex;

			vertex.Position = { scene->mMeshes[i]->mVertices[j].x,
								 scene->mMeshes[i]->mVertices[j].y,
								 scene->mMeshes[i]->mVertices[j].z };

			if (scene->mMeshes[i]->mTextureCoords[0])
			{
				const float uv_X = scene->mMeshes[i]->mTextureCoords[0][j].x;
				const float uv_Y = scene->mMeshes[i]->mTextureCoords[0][j].y;
				vertex.TexCoords = glm::vec2(uv_X, uv_Y);
			}
			else
			{
				vertex.TexCoords = glm::vec2(0.0f, 0.0f);
			}

			Vertices.push_back(vertex);
		}

		Indices.reserve((size_t)scene->mMeshes[i]->mNumFaces * 3);

		for (uint32_t j = 0; j < scene->mMeshes[i]->mNumFaces; j++)
		{
			for (uint32_t k = 0; k < scene->mMeshes[i]->mFaces[j].mNumIndices; k++)
			{
				Indices.push_back(scene->mMeshes[i]->mFaces[j].mIndices[k]);
			}
		}

		const aiMesh* model = scene->mMeshes[i];
		const aiMaterial* mtl = scene->mMaterials[model->mMaterialIndex];

		Texture2D* baseColorTexture = nullptr;
		// Base Color textures
		{
			aiString texpath;	// filename
			aiReturn texFound = mtl->GetTexture(aiTextureType_DIFFUSE, 0, &texpath);

			if (texFound == AI_SUCCESS)
			{
				material.m_BaseColorTexture = Texture2D::Create(path.remove_filename().string() + texpath.C_Str());
				std::cout << "[ASSIMP] Found texture: " << texpath.C_Str() << "\n";
			}
		}

		retMeshes.push_back(MakeShared<Mesh>(Vertices, Indices, material));

		std::cout << "[ASSIMP] Successfully imported mesh: " << scene->mMeshes[i]->mName.C_Str() << "\n";
	}

	return retMeshes;
}

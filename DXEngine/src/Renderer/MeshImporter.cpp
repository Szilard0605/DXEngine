#include "MeshImporter.h"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include "Math/Math.h"
#include "Mesh.h"
#include <string>
#include <print>

// bad hack, but it is what it is
aiMatrix4x4 GetMeshTransform(aiNode* node, uint32_t meshIndex, aiMatrix4x4 parentTransform = aiMatrix4x4()) 
{
	aiMatrix4x4 currentTransform = parentTransform * node->mTransformation;
	for (uint32_t i = 0; i < node->mNumMeshes; i++) 
	{
		if (node->mMeshes[i] == meshIndex) return currentTransform;
	}
	for (uint32_t i = 0; i < node->mNumChildren; i++) 
	{
		aiMatrix4x4 found = GetMeshTransform(node->mChildren[i], meshIndex, currentTransform);
		if (!found.IsIdentity()) return found;
	}
	return aiMatrix4x4();
}

glm::mat4 aiMatrix4x4ToGlm(const aiMatrix4x4& from) 
{
	glm::mat4 to;
	to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
	to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
	to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
	to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;
	return to;
}

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
		Mesh newMesh;
		PBRMaterial material{};
	
		newMesh.SetName(scene->mMeshes[i]->mName.C_Str());

		aiMatrix4x4 meshTransform = GetMeshTransform(scene->mRootNode, i);
		glm::mat4 mtransform = aiMatrix4x4ToGlm(meshTransform); // Konvertáld GLM-re

		Math::Transform transform;
		glm::quat rotation;
		Math::DecomposeTransform(mtransform, transform.Position, rotation, transform.Scale);
		transform.Rotation = glm::eulerAngles(rotation);
		newMesh.SetTransform(transform);

		printf("[ASSIMP] Mesh Transform Position: X: %f, Y: %f, Z: %f\n", transform.Position.x, transform.Position.y, transform.Position.z);

		for (uint32_t j = 0; j < scene->mMeshes[i]->mNumVertices; j++)
		{
			MeshVertex vertex;

			vertex.Position = { scene->mMeshes[i]->mVertices[j].x,
								scene->mMeshes[i]->mVertices[j].y,
							    scene->mMeshes[i]->mVertices[j].z };

			vertex.Normal = { scene->mMeshes[i]->mNormals[j].x,
							  scene->mMeshes[i]->mNormals[j].y,
							  scene->mMeshes[i]->mNormals[j].z };

			if (scene->mMeshes[i]->HasTangentsAndBitangents())
			{
				vertex.Tangent = { scene->mMeshes[i]->mTangents[j].x,
								   scene->mMeshes[i]->mTangents[j].y,
								   scene->mMeshes[i]->mTangents[j].z };

				vertex.Bitangent = { scene->mMeshes[i]->mBitangents[j].x,
									 scene->mMeshes[i]->mBitangents[j].y,
									 scene->mMeshes[i]->mBitangents[j].z };
			}


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

		aiColor3D color(0.f, 0.f, 0.f);
		if (mtl->Get(AI_MATKEY_COLOR_DIFFUSE, color))
		{
			material.m_BaseColor = { color.r, color.g, color.b };
			printf("[ASSIMP] Material base color: R: %f, G: %f, B: %f\n", color.r, color.g, color.b);
		}

		float metallic = 0.0f;
		if(mtl->Get(AI_MATKEY_METALLIC_FACTOR, metallic) == AI_SUCCESS)
		{
			material.m_Metallic = metallic;
			printf("[ASSIMP] Material metallic: %f\n", metallic);
		}

		float roughness = 1.0f;
		if (mtl->Get(AI_MATKEY_ROUGHNESS_FACTOR, roughness) == AI_SUCCESS)
		{
			material.m_Roughness = roughness;
			printf("[ASSIMP] Material roughness: %f\n", roughness);
		}

		Texture2D* baseColorTexture = nullptr;
		// Base Color textures
		{
			aiString texpath;	// filename
			aiReturn texFound = mtl->GetTexture(aiTextureType_DIFFUSE, 0, &texpath);

			if (texFound == AI_SUCCESS)
			{
				Texture2DProperties props;
				props.sourcePath = path.remove_filename().string() + texpath.C_Str();
				material.BaseColorTexture = Texture2D::Create(props);
				std::cout << "[ASSIMP] Found albedo texture: " << texpath.C_Str() << "\n";
			}
		}

		Texture2D* normalTexture = nullptr;
		// Normal Map textures
		{
			aiString texpath;	// filename
			aiReturn texFound = mtl->GetTexture(aiTextureType_NORMALS, 0, &texpath);
			if (texFound == AI_SUCCESS)
			{
				Texture2DProperties props;
				props.sourcePath = path.remove_filename().string() + texpath.C_Str();
				material.NormalTexture = Texture2D::Create(props);
				std::cout << "[ASSIMP] Found Normal map texture: " << texpath.C_Str() << "\n";
			}
		}

		
		// Metallic-Roughness textures
		{
			aiString texpath;	// filename
			aiReturn texFound = mtl->GetTexture(aiTextureType_UNKNOWN, 0, &texpath);
			if (texFound == AI_SUCCESS)
			{
				Texture2DProperties props;
				props.sourcePath = path.remove_filename().string() + texpath.C_Str();
				material.MetallicRoughnessTexture = Texture2D::Create(props);
				std::cout << "[ASSIMP] Found Metallic map texture: " << texpath.C_Str() << "\n";
			}
		}

		newMesh.SetVertices(Vertices);
		newMesh.SetIndices(Indices);
		newMesh.SetMaterial(material);
		newMesh.InitializeBuffers();
		retMeshes.push_back(MakeShared<Mesh>(newMesh));

		std::cout << "[ASSIMP] Successfully imported mesh: " << scene->mMeshes[i]->mName.C_Str() << "\n";
	}

	return retMeshes;
}

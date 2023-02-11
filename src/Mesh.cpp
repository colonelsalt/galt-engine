#include "Mesh.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "NameTag.h"

constexpr char* MODELS_DIRECTORY = "assets/models/";

static glm::mat4 AssimpToGlmMatrix(const aiMatrix4x4& assimpMatrix)
{
	glm::mat4 out;

	for (uint32_t i = 0; i < 4; i++)
	{
		for (uint32_t j = 0; j < 4; j++)
		{
			out[i][j] = assimpMatrix[j][i];
		}
	}
	return out;
}

static void LoadTextureFromMaterial(aiMaterial* material,
                                    const aiScene* scene,
                                    Mesh* outMesh,
                                    char* directoryPath)
{
	constexpr aiTextureType TEXTURE_TYPES[] =
	{
		aiTextureType_DIFFUSE, aiTextureType_SPECULAR, aiTextureType_NORMALS
	};

	for (uint32_t i = 0; i < ArrayCount(TEXTURE_TYPES); i++)
	{
		aiTextureType texType = TEXTURE_TYPES[i];
		for (uint32_t j = 0; j < material->GetTextureCount(texType); j++)
		{
			aiString path;
			material->GetTexture(texType, j, &path);
		
			MeshTexture* texture = &outMesh->a_Textures[outMesh->NumTextures++];
			texture->Type = (TextureType)texType;

			TextureParams texParams = {};
			texParams.FlipVertically = true;
			texParams.WrappingOption = TextureParams::DefaultWrapping();

			if (scene->mNumTextures > 0)
			{
				// Textures are embedded in model file
				const aiTexture* aiTexture = scene->GetEmbeddedTexture(path.data);
				texture->TextureId = LoadTextureEmbedded(aiTexture, &texParams);
			}
			else
			{
				// Textures are in an external file
				texParams.FileName = path.data;
				texParams.DirectoryPath = directoryPath;

				texture->TextureId = LoadTexture(&texParams);
			}
		}
	}
}

static void ParseMesh(aiMesh* assimpMesh, 
                      const aiScene* scene,
                      Entity entity,
                      char* directoryPath)
{
	Mesh* outMesh = entity.AddComponent<Mesh>();
	Assert(outMesh);

	outMesh->NumVertices = assimpMesh->mNumVertices;
	outMesh->NumIndices = 3 * assimpMesh->mNumFaces;

	// TODO: Is this necessary if we're just sending them to the GPU?
	outMesh->a_Vertices = (Vertex*)
		g_Memory->TempAlloc(outMesh->NumVertices * sizeof(Vertex));
	outMesh->a_Indices = (uint32_t*)
		g_Memory->TempAlloc(outMesh->NumIndices * sizeof(uint32_t));

	// Parse vertex data
	for (uint32_t i = 0; i < assimpMesh->mNumVertices; i++)
	{
		Vertex* ourVertex = &outMesh->a_Vertices[i];
		
		aiVector3D* aiPos = &assimpMesh->mVertices[i];
		aiVector3D* aiNormal = &assimpMesh->mNormals[i];
		aiVector3D* aiTangent = &assimpMesh->mTangents[i];
		aiVector3D* aiBitangent = &assimpMesh->mBitangents[i];

		ourVertex->Position = { aiPos->x, aiPos->y, aiPos->z };
		ourVertex->Normal = { aiNormal->x, aiNormal->y, aiNormal->z };
		ourVertex->Tangent = { aiTangent->x, aiTangent->y, aiTangent->z };
		ourVertex->Bitangent = { aiBitangent->x, aiBitangent->y, aiBitangent->z };

		// TODO: Work out whether we need to process the other texcoords in this array
		if (assimpMesh->mTextureCoords[0])
		{
			aiVector3D* aiTexCoords = &assimpMesh->mTextureCoords[0][i];
			ourVertex->TexCoords = { aiTexCoords->x, aiTexCoords->y };
		}
		else
		{
			ourVertex->TexCoords = { 0.0f, 0.0f };
		}
	}

	// Parse face index data
	for (uint32_t i = 0; i < assimpMesh->mNumFaces; i++)
	{
		aiFace* face = &assimpMesh->mFaces[i];
		Assert(face && face->mNumIndices == 3);
		for (uint32_t j = 0; j < face->mNumIndices; j++)
		{
			outMesh->a_Indices[i * 3 + j] = face->mIndices[j];
		}
	}

	if (assimpMesh->mMaterialIndex >= 0)
	{
		outMesh->NumTextures = 0;
		aiMaterial* material = scene->mMaterials[assimpMesh->mMaterialIndex];

		uint32_t totalTextures = material->GetTextureCount(aiTextureType_DIFFUSE)
			+ material->GetTextureCount(aiTextureType_SPECULAR)
			+ material->GetTextureCount(aiTextureType_NORMALS);
		outMesh->a_Textures = (MeshTexture*)
			g_Memory->TempAlloc(totalTextures * sizeof(MeshTexture));
		LoadTextureFromMaterial(material, scene, outMesh, directoryPath);
	}

	glGenVertexArrays(1, &outMesh->VertexArrayId);
	glBindVertexArray(outMesh->VertexArrayId);

	uint32_t vertexBufferId;
	glGenBuffers(1, &vertexBufferId);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);

	uint32_t indexBufferId;
	glGenBuffers(1, &indexBufferId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId);

	glBufferData(GL_ARRAY_BUFFER,
	             outMesh->NumVertices * sizeof(Vertex),
	             outMesh->a_Vertices,
	             GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
	             outMesh->NumIndices * sizeof(uint32_t),
	             outMesh->a_Indices,
	             GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));

	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));
}

static void ParseNode(aiNode* node,
                      const aiScene* scene,
                      Entity entity,
                      char* directoryPath)
{
	Transform* transform = entity.Trans();
	Assert(transform);
	transform->Model = AssimpToGlmMatrix(node->mTransformation);
	transform->AllocChildren(node->mNumChildren);

	for (uint32_t i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* assimpMesh = scene->mMeshes[node->mMeshes[i]];
		Assert(assimpMesh);

		// TODO: Handle submeshes properly
		ParseMesh(assimpMesh, scene, entity, directoryPath);
	}

	for (uint32_t i = 0; i < node->mNumChildren; i++)
	{
		char* childName = node->mName.data;
		Entity childEntity = g_EntityMaster->CreateEntity(childName);

		ParseNode(node->mChildren[i], scene, childEntity, directoryPath);
		transform->a_Children[i] = childEntity.Trans();
		transform->a_Children[i]->p_Parent = transform;
	}
}

static Entity LoadMesh(const char* modelName)
{
	char filePath[PATH_MAX];
	CatStr(MODELS_DIRECTORY, modelName, filePath);

	Assimp::Importer importer;
	int flags = aiProcess_Triangulate | aiProcess_CalcTangentSpace;
	const aiScene* scene = importer.ReadFile(filePath, flags);
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		Assert(false); // Failed to load model
	}

	// TODO: Handle constructing hierarchy of submeshes
	//Assert(scene->mNumMeshes == 1);

	// Extract directory name & file name without extension
	int lastSlashIndex = LastIndexOf(filePath, '/');
	char* directoryPath = filePath;

	char rootMeshName[MAX_NAME_LENGTH];
	int lastDotIndex = LastIndexOf(filePath, '.');
	Substring(filePath, lastSlashIndex + 1, lastDotIndex, rootMeshName);

	directoryPath[lastSlashIndex + 1] = 0;

	Entity rootEntity = g_EntityMaster->CreateEntity(rootMeshName);
	ParseNode(scene->mRootNode, scene, rootEntity, filePath);
	return rootEntity;
}

void Mesh::Draw()
{
	glBindVertexArray(VertexArrayId);
	
	if (!p_Shader)
	{
		// TODO: Use default flat-colour shader if none assigned
		Assert(false);
	}
	
	p_Shader->Bind();

	Assert(ThisEntity);
	Transform* transform = ThisEntity.Trans();
	Assert(transform);

	p_Shader->SetMat4("u_Model", transform->WorldSpace());
	
	for (uint32_t i = 0; i < NumTextures; i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		MeshTexture* texture = &a_Textures[i];
		glBindTexture(GL_TEXTURE_2D, texture->TextureId);
		
		if (texture->Type == TextureType::DIFFUSE)
		{
			p_Shader->SetInt("u_DiffuseTexture", i);
		}
		else if (texture->Type == TextureType::SPECULAR)
		{
			p_Shader->SetInt("u_SpecTexture", i);
		}
		else if (texture->Type == TextureType::NORMAL)
		{
			p_Shader->SetInt("u_NormalTexture", i);
		}
	}

	glDrawElements(GL_TRIANGLES, NumIndices, GL_UNSIGNED_INT, nullptr);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

static void SetShaderInHierarchy(Transform* transform, Shader* shader)
{
	Assert(transform && shader);
	Mesh* mesh = transform->ThisEntity.GetComponent<Mesh>();
	if (mesh)
	{
		mesh->p_Shader = shader;
	}
	for (uint32_t i = 0; i < transform->NumChildren; i++)
	{
		SetShaderInHierarchy(transform->a_Children[i], shader);
	}
}

void Mesh::SetShader(Shader* shader)
{
	Assert(shader);
	SetShaderInHierarchy(ThisEntity.Trans(), shader);
}
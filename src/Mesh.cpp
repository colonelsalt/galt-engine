#include "Mesh.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

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
	aiTextureType texType = aiTextureType_DIFFUSE;
	for (uint32_t i = 0; i < material->GetTextureCount(texType); i++)
	{
		aiString path;
		material->GetTexture(texType, i, &path);
		
		MeshTexture* texture = &outMesh->a_Textures[outMesh->NumTextures++];

		TextureParams texParams = {};
		texParams.FileName = path.data;
		texParams.DirectoryPath = directoryPath;
		texParams.FlipVertically = true;
		texParams.WrappingOption = TextureParams::DefaultWrapping();

		texture->TextureId = LoadTexture(&texParams);
		texture->Type = TextureType::DIFFUSE;
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

		ourVertex->Position = { aiPos->x, aiPos->y, aiPos->z };
		ourVertex->Normal = { aiNormal->x, aiNormal->y, aiNormal->z };

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

		uint32_t totalTextures = material->GetTextureCount(aiTextureType_DIFFUSE);
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
}

static void ParseNode(aiNode* node,
                      const aiScene* scene,
                      Entity entity,
                      char* directoryPath)
{
	Transform* transform = entity.AddComponent<Transform>();
	Assert(transform);
	transform->CompInit(node->mNumChildren);

	for (uint32_t i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* assimpMesh = scene->mMeshes[node->mMeshes[i]];
		Assert(assimpMesh);

		// TODO: Handle submeshes properly
		ParseMesh(assimpMesh, scene, entity, directoryPath);
	}

	for (uint32_t i = 0; i < node->mNumChildren; i++)
	{
		Entity childEntity = g_EntityMaster->CreateEntity();

		ParseNode(node->mChildren[i], scene, childEntity, directoryPath);
		transform->a_Children[i] = childEntity.GetComponent<Transform>();
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
	Assert(scene->mNumMeshes == 1);

	// Get directory name
	int lastSlashIndex = LastIndexOf(filePath, '/');
	filePath[lastSlashIndex + 1] = 0;

	Entity rootEntity = g_EntityMaster->CreateEntity();
	ParseNode(scene->mRootNode, scene, rootEntity, filePath);
	return rootEntity;
}

void Mesh::Draw(Shader* shader)
{
	glBindVertexArray(VertexArrayId);
	shader->Bind();

	Entity entity = { EntityId };
	Assert(entity);
	Transform* transform = entity.GetComponent<Transform>();
	Assert(transform);

	shader->SetMat4("u_Model", transform->WorldSpace());

	Assert(NumTextures == 1);
	glBindTexture(GL_TEXTURE_2D, a_Textures[0].TextureId);

	glDrawElements(GL_TRIANGLES, NumIndices, GL_UNSIGNED_INT, nullptr);
}
#pragma once

#include <glm/glm.hpp>

#include "HashMap.h"
#include "Renderer.h"

static constexpr int MAX_BONES_PER_VERTEX = 4;
static constexpr int MAX_TOTAL_BONES = 100;

struct Vertex
{
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
	glm::vec3 Tangent;
	glm::vec3 Bitangent;

	// ID of each bone whose transform influences the pos/rot of this vertex
	int BoneIds[MAX_BONES_PER_VERTEX];
	// How much (in range [0, 1]) those bones affect the pos/rot of this vertex
	float BoneWeights[MAX_BONES_PER_VERTEX];

	inline void ResetBoneData()
	{
		for (int i = 0; i < MAX_BONES_PER_VERTEX; i++)
		{
			BoneIds[i] = -1;
			BoneWeights[i] = 0.0f;
		}
	}

	inline void SetBoneInfluence(int boneId, float weight)
	{
		for (int i = 0; i < MAX_BONES_PER_VERTEX; i++)
		{
			if (BoneIds[i] == -1)
			{
				BoneIds[i] = boneId;
				BoneWeights[i] = weight;
				break;
			}
		}
	}

};

// Defining values here to correspond with Assimp's texture type enum for easy conversion
enum TextureType
{
	DIFFUSE = 1,
	SPECULAR = 2,
	NORMAL = 6,
	SHADOW_MAP = 20
};

enum MeshType
{
	STATIC = 0,
	SKINNED = 1
};

struct MeshTexture
{
	uint32_t TextureId;
	TextureType Type;
};

struct Bone
{
	int Id;

	glm::mat4 InverseBindPose;
};

struct Skeleton
{
	uint32_t NumBones;
	Bone Bones[MAX_TOTAL_BONES];
	int EntityToBoneId[MAX_ENTITIES];

	inline void Init()
	{
		for (uint32_t i = 0; i < MAX_ENTITIES; i++)
		{
			EntityToBoneId[i] = -1;
		}
	}
};

struct Mesh : public Component
{
	uint32_t VertexArrayId;

	uint32_t NumVertices;
	uint32_t NumIndices;
	
	// TODO: Do we actually need to store these on the CPU-side at all?
	Vertex* a_Vertices;
	uint32_t* a_Indices;

	uint32_t NumTextures;
	MeshTexture* a_Textures;

	Shader* p_Shader;
	Shader* p_ShadowShader;

	Skeleton* p_Skeleton;

	COMPONENT_DEF(ComponentType::MESH, MAX_MESHES);

	void Draw(RenderPass renderPass, uint32_t shadowMapTextureId = 0);

	void SetShader(Shader* shader);
};
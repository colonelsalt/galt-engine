#pragma once

#include <glm/glm.hpp>

struct Vertex
{
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;

	// TODO: Bone data
};

enum class TextureType
{
	DIFFUSE, SPECULAR
};

struct MeshTexture
{
	uint32_t TextureId;
	TextureType Type;
};

struct Mesh
{
	glm::mat4 Trans;
	uint32_t VertexArrayId;

	uint32_t NumVertices;
	uint32_t NumIndices;
	
	// TODO: Do we actually need to store these on the CPU-side at all?
	Vertex* a_Vertices;
	uint32_t* a_Indices;

	uint32_t NumChildren;
	Mesh* a_Children;
	Mesh* p_Parent;

	uint32_t NumTextures;
	MeshTexture* a_Textures;

	void Draw(Shader* shader);
};
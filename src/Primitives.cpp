#include "Primitives.h"

#include <glad/glad.h>

#include "Textures.h"

void Primitive::Draw(uint32_t shadowMapTextureId)
{
	Assert(p_Shader); // TODO: Default shader if missing

	Transform* transform = ThisEntity.Trans();
	Assert(transform);

	glBindVertexArray(VertexArrayId);
	p_Shader->Bind();
	p_Shader->SetMat4("u_Model", transform->Local);
	
	int textureIndex = 0;
	glActiveTexture(GL_TEXTURE0 + textureIndex);
	if (DiffuseTextureId)
	{
		glBindTexture(GL_TEXTURE_2D, DiffuseTextureId);
		p_Shader->SetInt("u_DiffuseTexture", textureIndex++);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	glActiveTexture(GL_TEXTURE0 + textureIndex);
	if (SpecularTextureId)
	{
		glBindTexture(GL_TEXTURE_2D, SpecularTextureId);
		p_Shader->SetInt("u_UsesSpec", true);
		p_Shader->SetInt("u_SpecTexture", textureIndex++);
	}
	else
	{
		p_Shader->SetInt("u_UsesSpec", false);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	glActiveTexture(GL_TEXTURE0 + textureIndex);
	if (SkyboxTextureId)
	{
		glBindTexture(GL_TEXTURE_CUBE_MAP, SkyboxTextureId);
		p_Shader->SetInt("u_SkyboxTexture", textureIndex++);
		glCullFace(GL_FRONT);
	}
	else
	{
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	}
	glActiveTexture(GL_TEXTURE0 + textureIndex);
	if (shadowMapTextureId)
	{
		glBindTexture(GL_TEXTURE_2D, shadowMapTextureId);
		p_Shader->SetInt("u_ShadowMapTexture", textureIndex++);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	glDrawArrays(GL_TRIANGLES, 0, NumVertices);

	glCullFace(GL_BACK);
}

static uint32_t SetUpPrimitiveVertexArray(const float* vertices, size_t verticesSize)
{
	uint32_t vertexArrayId;
	glGenVertexArrays(1, &vertexArrayId);
	glBindVertexArray(vertexArrayId);

	uint32_t vertexBuffer;
	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, verticesSize, vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), nullptr);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));

	return vertexArrayId;
}

static Entity CreatePlane(char* entityName, char* textureName)
{
	Entity entity = g_EntityMaster->CreateEntity("Plane");

	Primitive* primitive = entity.AddComponent<Primitive>();

	primitive->DiffuseTextureId = LoadTexture(textureName);

	constexpr float vertices[] = 
	{
		// positions            // normals         // texcoords
		 25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,  15.0f,  0.0f,
		 -25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 15.0f,
		-25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,

		 25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,  15.0f,  0.0f,
		 25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,  15.0f, 15.0f,
		-25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 15.0f
	};
	primitive->NumVertices = ArrayCount(vertices) / 8;
	primitive->VertexArrayId = SetUpPrimitiveVertexArray(vertices, sizeof(vertices));

	return entity;
}

static void PopulateCube(Primitive* outCube,
                         char* diffuseTextureName = nullptr,
                         char* specularTextureName = nullptr)
{
	if (diffuseTextureName)
	{
		outCube->DiffuseTextureId = LoadTexture(diffuseTextureName);
	}
	if (specularTextureName)
	{
		outCube->SpecularTextureId = LoadTexture(specularTextureName);
	}

	constexpr float vertices[] = {
		// back face
		-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
		 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
		 1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
		 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
		-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
		-1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
		// front face
		-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
		 1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
		 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
		 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
		-1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
		-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
		// left face
		-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
		-1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
		-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
		-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
		-1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
		-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
		// right face
		 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
		 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
		 1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
		 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
		 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
		 1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
		// bottom face
		-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
		 1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
		 1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
		 1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
		-1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
		-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
		// top face
		-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
		 1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
		 1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
		 1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
		-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
		-1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
	};

	outCube->NumVertices = ArrayCount(vertices) / 8;
	outCube->VertexArrayId = SetUpPrimitiveVertexArray(vertices, sizeof(vertices));
}

static Entity CreateCube(char* entityName,
                         char* diffuseTextureName = nullptr,
                         char* specularTextureName = nullptr)
{
	Entity entity = g_EntityMaster->CreateEntity("Cube");

	Primitive* primitive = entity.AddComponent<Primitive>();
	Assert(primitive);

	PopulateCube(primitive, diffuseTextureName, specularTextureName);
	return entity;
}
#include "Lights.h"
#include "Animator.h"

static constexpr uint32_t SHADOW_MAP_WIDTH = 1024;
static constexpr uint32_t SHADOW_MAP_HEIGHT = 1024;

void RenderSetup(GameState* state)
{
	// ---
	// Uniform buffer declarations
	// ---

	// Set up ProjView uniform buffer
	glGenBuffers(1, &state->ProjViewUniformBuffer);
	glBindBuffer(GL_UNIFORM_BUFFER, state->ProjViewUniformBuffer);
	glBufferData(GL_UNIFORM_BUFFER,
	             2 * sizeof(glm::mat4),
	             nullptr,
	             GL_STATIC_DRAW);
	
	
	// Bind ProjView uniform buffer to uniform block at binding point 0
	glBindBufferRange(GL_UNIFORM_BUFFER,
	                  0,
	                  state->ProjViewUniformBuffer,
	                  0,
	                  2 * sizeof(glm::mat4));

	// Set up skinning matrices uniform buffer
	glGenBuffers(1, &state->SkinningMatricesUniformBuffer);
	glBindBuffer(GL_UNIFORM_BUFFER, state->SkinningMatricesUniformBuffer);
	glBufferData(GL_UNIFORM_BUFFER,
	             MAX_TOTAL_BONES * sizeof(glm::mat4),
	             nullptr,
	             GL_STATIC_DRAW);

	glBindBufferRange(GL_UNIFORM_BUFFER,
	                  1,
	                  state->SkinningMatricesUniformBuffer,
	                  0,
	                  MAX_TOTAL_BONES * sizeof(glm::mat4));

	// ---
	// Shadow map setup
	// ---
	glGenFramebuffers(1, &state->ShadowMapFramebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, state->ShadowMapFramebuffer);
	
	glGenTextures(1, &state->ShadowMapTextureId);
	glBindTexture(GL_TEXTURE_2D, state->ShadowMapTextureId);
	
	glTexImage2D(GL_TEXTURE_2D,
	             0,
	             GL_DEPTH_COMPONENT,
	             SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT,
	             0,
	             GL_DEPTH_COMPONENT,
	             GL_FLOAT,
	             nullptr);
	
	constexpr float borderColour[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColour);
	
	glFramebufferTexture2D(GL_FRAMEBUFFER,
	                       GL_DEPTH_ATTACHMENT,
	                       GL_TEXTURE_2D,
	                       state->ShadowMapTextureId,
	                       0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderScene(GameState* state)
{
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

	Animator* playerAnimator = state->Player.GetComponent<Animator>();
	Assert(playerAnimator);

	// Send skinning matrices to uniform buffer
	glBindBuffer(GL_UNIFORM_BUFFER, state->SkinningMatricesUniformBuffer);
	glBufferSubData(GL_UNIFORM_BUFFER,
	                0,
	                MAX_TOTAL_BONES * sizeof(glm::mat4),
	                playerAnimator->SkinningMatrices);
	Light* light = state->PointLight.GetComponent<Light>();
	Transform* lightTransform = state->PointLight.Trans();
	Assert(light && lightTransform && light->Type == LightType::POINT);

	uint32_t numMeshes = g_EntityMaster->NumComponents(ComponentType::MESH);
	Mesh* meshes = g_EntityMaster->GetAllComponents<Mesh>();

	uint32_t numPrimitives = g_EntityMaster->NumComponents(ComponentType::PRIMITIVE);
	Primitive* primitives = g_EntityMaster->GetAllComponents<Primitive>();

	// ---
	// Shadow pass ---
	// ---
	glViewport(0, 0, SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, state->ShadowMapFramebuffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	//glm::vec3 lightTargetPos = { 0.0f, 0.0f, 0.0f };
	LightMatrix lightMatrix = light->GetLightMatrix(state->Player.Trans()->Position());
	
	// Send light matrix to proj/view uniform buffer
	glBindBuffer(GL_UNIFORM_BUFFER, state->ProjViewUniformBuffer);
	glBufferSubData(GL_UNIFORM_BUFFER,
	                0,
	                2 * sizeof(glm::mat4),
	                &lightMatrix);
	
	for (uint32_t i = 0; i < numMeshes; i++)
	{
		meshes[i].Draw(RenderPass::SHADOWS);
	}

	// ---
	// Standard render pass ---
	// ---
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, g_Memory->ScreenWidth, g_Memory->ScreenHeight);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Send projection matrix to uniform buffer
	glBindBuffer(GL_UNIFORM_BUFFER, state->ProjViewUniformBuffer);
	glBufferSubData(GL_UNIFORM_BUFFER,
	                0,
	                sizeof(glm::mat4),
	                glm::value_ptr(state->FpsCamera.Projection));

	// Send view matrix to uniform buffer
	glBufferSubData(GL_UNIFORM_BUFFER,
	                sizeof(glm::mat4),
	                sizeof(glm::mat4),
	                glm::value_ptr(state->FpsCamera.View));


	state->SkyboxShader.Bind();
	state->SkyboxShader.SetMat4("u_TrimmedView", glm::mat4(glm::mat3(state->FpsCamera.View)));

	state->FlatColourShader.Bind();
	state->FlatColourShader.SetVec3("u_Colour", light->Colour);

	state->BasicPhongShader.Bind();
	state->BasicPhongShader.SetVec3("u_CameraPos", state->FpsCamera.Position);
	state->BasicPhongShader.SetVec3("u_PointLight.Position", *lightTransform->Position());
	state->BasicPhongShader.SetVec3("u_PointLight.Colour", light->Colour);
	state->BasicPhongShader.SetMat4("u_LightProj", lightMatrix.Projection);
	state->BasicPhongShader.SetMat4("u_LightView", lightMatrix.View);

	state->MeshShader.Bind();
	state->MeshShader.SetVec3("u_CameraPos", state->FpsCamera.Position);
	state->MeshShader.SetVec3("u_PointLight.Position", *lightTransform->Position());
	state->MeshShader.SetVec3("u_PointLight.Colour", light->Colour);

	state->AnimShader.Bind();
	state->AnimShader.SetVec3("u_CameraPos", state->FpsCamera.Position);
	state->AnimShader.SetVec3("u_PointLight.Position", *lightTransform->Position());
	state->AnimShader.SetVec3("u_PointLight.Colour", light->Colour);

	for (uint32_t i = 0; i < numMeshes; i++)
	{
		meshes[i].Draw(RenderPass::STANDRD);
	}

	for (uint32_t i = 0; i < numPrimitives; i++)
	{
		primitives[i].Draw(state->ShadowMapTextureId);
	}
}
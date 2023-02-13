#include "Lights.h"

void RenderSetup(GameState* state)
{
	//glEnable(GL_MULTISAMPLE);

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

	// Send projection matrix to uniform buffer
	glBufferSubData(GL_UNIFORM_BUFFER,
	                0,
	                sizeof(glm::mat4),
	                glm::value_ptr(state->FpsCamera.Projection));
}

void RenderScene(GameState* state)
{
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Send view matrix to uniform buffer
	glBindBuffer(GL_UNIFORM_BUFFER, state->ProjViewUniformBuffer);
	glBufferSubData(GL_UNIFORM_BUFFER,
	                sizeof(glm::mat4),
	                sizeof(glm::mat4),
	                glm::value_ptr(state->FpsCamera.View));
	Light* light = state->PointLight.GetComponent<Light>();
	Transform* lightTransform = state->PointLight.Trans();
	Assert(light && lightTransform && light->Type == LightType::POINT);

	state->FlatColourShader.Bind();
	state->FlatColourShader.SetVec3("u_Colour", light->Colour);

	state->BasicPhongShader.Bind();
	state->BasicPhongShader.SetVec3("u_CameraPos", state->FpsCamera.Position);

	state->BasicPhongShader.SetVec3("u_PointLight.Position", *lightTransform->Position());
	state->BasicPhongShader.SetVec3("u_PointLight.Colour", light->Colour);

	state->MeshShader.Bind();
	state->MeshShader.SetVec3("u_CameraPos", state->FpsCamera.Position);

	state->MeshShader.SetVec3("u_PointLight.Position", *lightTransform->Position());
	state->MeshShader.SetVec3("u_PointLight.Colour", light->Colour);

	state->AnimShader.Bind();
	state->AnimShader.SetVec3("u_CameraPos", state->FpsCamera.Position);

	state->AnimShader.SetVec3("u_PointLight.Position", *lightTransform->Position());
	state->AnimShader.SetVec3("u_PointLight.Colour", light->Colour);


	uint32_t numMeshes = g_EntityMaster->NumComponents(ComponentType::MESH);
	Mesh* meshes = g_EntityMaster->GetAllComponents<Mesh>();
	for (uint32_t i = 0; i < numMeshes; i++)
	{
		meshes[i].Draw();
	}

	uint32_t numPrimitives = g_EntityMaster->NumComponents(ComponentType::PRIMITIVE);
	Primitive* primitives = g_EntityMaster->GetAllComponents<Primitive>();
	for (uint32_t i = 0; i < numPrimitives; i++)
	{
		primitives[i].Draw();
	}
}
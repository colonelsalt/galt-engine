#include "Galt.h"

#include "glad/glad.c"
#include "Shader.cpp"
#include "Primitives.cpp"
#include "Camera.cpp"

extern "C" void GAME_API UpdateAndRender(GameMemory* memory, ControllerInput* input)
{
	Assert(sizeof(GameState) <= memory->PermStorageSize);
	
	if (!memory->OpenGlInitialised)
	{
		gladLoadGLLoader(memory->GladLoader);
		memory->OpenGlInitialised = true;
	}
	if (memory->PendingScreenResize)
	{
		glViewport(0, 0, memory->ScreenWidth, memory->ScreenHeight);
		memory->PendingScreenResize = false;
	}

	GameState* state = (GameState*)memory->PermStorage;
	if (!memory->IsInitialised)
	{
		glEnable(GL_DEPTH_TEST);

		Shader* primitiveShader = &state->PrimitiveShader;
		primitiveShader->CompileProgram("PrimitiveVert.glsl",
		                                "PrimitiveFrag.glsl",
		                                memory);
		state->FpsCamera = CreateCamera();
		state->Plane = CreatePlane(primitiveShader);
		state->Cube = CreateCube(primitiveShader);
		memory->IsInitialised = true;
	}

	memory->ResetTempMemory();
	for (int i = 0; i < ArrayCount(state->Shaders); i++)
	{
		Shader* shader = &state->Shaders[i];
		shader->ReloadIfNeeded(memory);
	}

	state->FpsCamera.Update(input);

	// Rendering ---
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBindVertexArray(state->Plane.VertexArrayId);
	state->PrimitiveShader.Bind();

	state->PrimitiveShader.SetMat4("u_Projection", state->FpsCamera.Projection);
	state->PrimitiveShader.SetMat4("u_View", state->FpsCamera.View);
	state->PrimitiveShader.SetMat4("u_Model", state->Plane.Model);

	state->PrimitiveShader.SetVec3("u_Colour", { 1.0f, 0.0f, 0.0f });
	glDrawArrays(GL_TRIANGLES, 0, state->Plane.NumVertices);

	glBindVertexArray(state->Cube.VertexArrayId);
	state->PrimitiveShader.SetVec3("u_Colour", { 1.0f, 1.0f, 0.0f });
	glDrawArrays(GL_TRIANGLES, 0, state->Cube.NumVertices);

#if 0
	glBindVertexArray(state->TestVertexArrayId);
	state->TestShader.Bind();
	state->TestShader.SetVec2("u_Offset", state->PlayerPos);

	glDrawArrays(GL_TRIANGLES, 0, 3);

#endif
	glBindVertexArray(0);

}
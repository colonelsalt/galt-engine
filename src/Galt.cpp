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
		constexpr float vertices[] = {
			-0.5f, 0.0f, 0.0f,
			 0.0f, 0.5f, 0.0f,
			 0.5f, 0.0f, 0.0f		
		};

		glGenVertexArrays(1, &state->TestVertexArrayId);

		glBindVertexArray(state->TestVertexArrayId);

		uint32_t vertexBuffer;
		glGenBuffers(1, &vertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);

		Shader* testShader = &state->TestShader;
		testShader->CompileProgram("BasicVert.glsl", "BasicFrag.glsl", memory);

		Shader* primitiveShader = &state->PrimitiveShader;
		primitiveShader->CompileProgram("PrimitiveVert.glsl",
		                                "PrimitiveFrag.glsl",
		                                memory);
		state->FpsCamera = CreateCamera();
		state->Plane = CreatePlane(testShader);
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
	glClear(GL_COLOR_BUFFER_BIT);

	glBindVertexArray(state->Plane.VertexArrayId);
	state->PrimitiveShader.Bind();

	state->PrimitiveShader.SetMat4("u_Projection", state->FpsCamera.Projection);
	state->PrimitiveShader.SetMat4("u_View", state->FpsCamera.View);
	state->PrimitiveShader.SetMat4("u_Model", state->Plane.Model);

	glDrawArrays(GL_TRIANGLES, 0, state->Plane.NumVertices);

#if 0
	glBindVertexArray(state->TestVertexArrayId);
	state->TestShader.Bind();
	state->TestShader.SetVec2("u_Offset", state->PlayerPos);

	glDrawArrays(GL_TRIANGLES, 0, 3);

#endif
	glBindVertexArray(0);

}
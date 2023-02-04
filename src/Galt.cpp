#include "Galt.h"

#include <glad/glad.h>

#include "Shader.cpp"

extern "C" void GAME_API UpdateAndRender(GameMemory* memory, ControllerInput* input)
{
	Assert(sizeof(GameState) <= memory->PermStorageSize);
	
	if (!memory->OpenGlInitialised)
	{
		gladLoadGLLoader(memory->GladLoader);
		memory->OpenGlInitialised = true;
	}

	GameState* state = (GameState*)memory->PermStorage;
	if (!memory->IsInitialised)
	{
		constexpr float vertices[] = {
			-0.5f, 0.0f, 0.0f,
			 0.0f, 0.5f, 0.0f,
			 0.5f, 0.0f, 0.0f		
		};
		uint32_t vao;

		glGenVertexArrays(1, &vao);

		state->VertexArrayId = vao;

		glBindVertexArray(state->VertexArrayId);

		uint32_t vertexBuffer;
		glGenBuffers(1, &vertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);

		state->TestShader.RendererId =
			ShaderCompileProgram("BasicVert.glsl", "BasicFrag.glsl", memory);

		memory->IsInitialised = true;
	}

	memory->ResetTempMemory();

	constexpr float moveSpeed = 0.001f;
	if (input->Up)
	{
		state->PlayerY += moveSpeed;
	}
	if (input->Down)
	{
		state->PlayerY -= moveSpeed;
	}
	if (input->Left)
	{
		state->PlayerX -= moveSpeed;
	}
	if (input->Right)
	{
		state->PlayerX += moveSpeed;
	}
	
	// Clamping
	if (state->PlayerX > 1.0f)
	{
		state->PlayerX = 1.0f;
	}
	if (state->PlayerX < -1.0f)
	{
		state->PlayerX = -1.0f;
	}
	if (state->PlayerY > 1.0f)
	{
		state->PlayerY = 1.0f;
	}
	if (state->PlayerY < -1.0f)
	{
		state->PlayerY = -1.0f;
	}

	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glBindVertexArray(state->VertexArrayId);
	state->TestShader.Bind();
	state->TestShader.SetFloat("u_OffsetX", state->PlayerX);
	state->TestShader.SetFloat("u_OffsetY", state->PlayerY);

	glDrawArrays(GL_TRIANGLES, 0, 3);

	glBindVertexArray(0);

}
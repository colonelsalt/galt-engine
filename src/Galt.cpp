#include "Galt.h"

#include <glad/glad.h>

void UpdateAndRender(GameMemory* memory, ControllerInput* input)
{
	Assert(sizeof(GameState) <= memory->PermStorageSize);
	
	GameState* state = (GameState*)memory->PermStorage;
	if (!memory->IsInitialised)
	{
		constexpr float vertices[] = {
			-0.5f, 0.0f, 0.0f,
			 0.0f, 0.5f, 0.0f,
			 0.5f, 0.0f, 0.0f		
		};

		glGenVertexArrays(1, &state->VertexArrayId);
		glBindVertexArray(state->VertexArrayId);

		uint32_t vertexBuffer;
		glGenBuffers(1, &vertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);

		state->ClearR = 1.0f;
		state->ClearG = 1.0f;
		state->ClearB = 0.0f;

		memory->IsInitialised = true;
	}

	constexpr float colourSpeed = 0.001f;
	if (input->Up)
	{
		state->ClearR += colourSpeed;
	}
	if (input->Down)
	{
		state->ClearR -= colourSpeed;
	}
	if (input->Left)
	{
		state->ClearG -= colourSpeed;
	}
	if (input->Right)
	{
		state->ClearG += colourSpeed;
	}
	if (state->ClearR < 0.0f)
	{
		state->ClearR = 0.0f;
	}
	if (state->ClearR > 1.0f)
	{
		state->ClearR = 1.0f;
	}
	if (state->ClearG < 0.0f)
	{
		state->ClearG = 0.0f;
	}
	if (state->ClearG > 1.0f)
	{
		state->ClearG = 1.0f;
	}

	glClearColor(state->ClearR, state->ClearG, state->ClearB, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glBindVertexArray(state->VertexArrayId);

	glDrawArrays(GL_TRIANGLES, 0, 3);

	glBindVertexArray(0);

}
#pragma once

#include <glad/glad.h>

struct GameMemory;

struct Shader
{
	uint32_t RendererId;

	inline void Bind()
	{
		glUseProgram(RendererId);
	}

	void SetInt(const char* name, int value);
	void SetFloat(const char* name, float value);
};
#pragma once

#include <glad/glad.h>

#include "Defines.h"

struct GameMemory;

struct Shader
{
	uint32_t RendererId;

	char VertexSourcePath[PATH_MAX];
	int64_t VertexLastWriteTime;
	char FragmentSourcePath[PATH_MAX];
	int64_t FragmentLastWriteTime;

	void CompileProgram(const char* vertexFileName,
	                    const char* fragmentFileName,
	                    GameMemory* gameMemory);

	void CompileProgram(GameMemory* gameMemory);

	void ReloadIfNeeded(GameMemory* memory);

	inline void Bind()
	{
		glUseProgram(RendererId);
	}

	void SetInt(const char* name, int value);
	void SetFloat(const char* name, float value);
};
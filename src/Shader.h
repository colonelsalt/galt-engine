#pragma once

#include <glm/glm.hpp>

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

	inline void Bind();

	void SetInt(const char* name, int value);
	void SetFloat(const char* name, float value);
	void SetVec2(const char* name, const glm::vec2& value);
	void SetVec3(const char* name, const glm::vec3& value);
	void SetMat4(const char* name, const glm::mat4& value);
};
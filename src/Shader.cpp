#include "Shader.h"

#include <glm/gtc/type_ptr.hpp>

#include "Util.h"

static constexpr char* SHADERS_DIR = "src/shaders/";

void Shader::ReloadIfNeeded()
{
	int64_t vertLastWriteTime = g_Memory->GetLastWriteTime(VertexSourcePath);
	int64_t fragLastWriteTime = g_Memory->GetLastWriteTime(FragmentSourcePath);

	if (vertLastWriteTime > VertexLastWriteTime
		|| fragLastWriteTime > FragmentLastWriteTime)
	{
		CompileProgram();
	}
}

inline void Shader::Bind()
{
	glUseProgram(RendererId);
}

static uint32_t CompileShader(const char* source, GLenum type)
{
	uint32_t shaderId = glCreateShader(type);

	glShaderSource(shaderId, 1, &source, nullptr);
	glCompileShader(shaderId);

	int compileSuccess;
	glGetShaderiv(shaderId, GL_COMPILE_STATUS, &compileSuccess);
	if (!compileSuccess)
	{
		char infoLog[512];
		glGetShaderInfoLog(shaderId, sizeof(infoLog), nullptr, infoLog);
		
		// TODO: Proper logging - shader failed to compile
		Assert(false);
	}
	return shaderId;
}


// First-time compilation
void Shader::CompileProgram(const char* vertexFileName,
                            const char* fragmentFileName)
{
	Assert(RendererId == 0);

	CatStr(SHADERS_DIR, vertexFileName, VertexSourcePath);
	CatStr(SHADERS_DIR, fragmentFileName, FragmentSourcePath);

	CompileProgram();
}


void Shader::CompileProgram()
{
	Assert(*VertexSourcePath && *FragmentSourcePath);

	FileResult vertexSourceFile = g_Memory->ReadFile(VertexSourcePath);
	FileResult fragmentSourceFile = g_Memory->ReadFile(FragmentSourcePath);
	VertexLastWriteTime = g_Memory->GetLastWriteTime(VertexSourcePath);
	FragmentLastWriteTime = g_Memory->GetLastWriteTime(FragmentSourcePath);

	uint32_t vertId = CompileShader((const char*)vertexSourceFile.Contents,
	                                GL_VERTEX_SHADER);
	uint32_t fragId = CompileShader((const char*)fragmentSourceFile.Contents,
	                                GL_FRAGMENT_SHADER);

	uint32_t shaderProgramId = glCreateProgram();

	glAttachShader(shaderProgramId, vertId);
	glAttachShader(shaderProgramId, fragId);

	glLinkProgram(shaderProgramId);

	int linkSuccess;
	glGetProgramiv(shaderProgramId, GL_LINK_STATUS, &linkSuccess);
	if (!linkSuccess)
	{
		char infoLog[512];
		glGetProgramInfoLog(shaderProgramId, sizeof(infoLog), nullptr, infoLog);

		// TODO: Logging - shader program failed to link
		Assert(false);
	}

	glDeleteShader(vertId);
	glDeleteShader(fragId);

	g_Memory->FreeFile(vertexSourceFile.Contents);
	g_Memory->FreeFile(fragmentSourceFile.Contents);

	if (RendererId)
	{
		// If we're re-compiling, delete the previous version of the shader program
		glDeleteProgram(RendererId);
	}
	RendererId = shaderProgramId;
}

void Shader::SetInt(const char* name, int value)
{
	int location = glGetUniformLocation(RendererId, name);
	glUniform1i(location, value);
}

void Shader::SetFloat(const char* name, float value)
{
	int location = glGetUniformLocation(RendererId, name);
	glUniform1f(location, value);
}

void Shader::SetVec2(const char* name, const glm::vec2& value)
{
	int location = glGetUniformLocation(RendererId, name);
	glUniform2fv(location, 1, glm::value_ptr(value));
}

void Shader::SetVec3(const char* name, const glm::vec3& value)
{
	int location = glGetUniformLocation(RendererId, name);
	glUniform3fv(location, 1, glm::value_ptr(value));
}

void Shader::SetMat4(const char* name, const glm::mat4& value)
{
	int location = glGetUniformLocation(RendererId, name);
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
}

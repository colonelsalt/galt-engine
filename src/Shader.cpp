#include "Shader.h"

static constexpr char* SHADERS_DIR = "src/shaders/";

void Shader::ReloadIfNeeded(GameMemory* memory)
{
	int64_t vertLastWriteTime = memory->GetLastWriteTime(VertexSourcePath);
	int64_t fragLastWriteTime = memory->GetLastWriteTime(FragmentSourcePath);

	if (vertLastWriteTime > VertexLastWriteTime
		|| fragLastWriteTime > FragmentLastWriteTime)
	{
		CompileProgram(memory);
	}
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
                            const char* fragmentFileName,
                            GameMemory* gameMemory)
{
	Assert(RendererId == 0);

	CatStr(SHADERS_DIR, vertexFileName, VertexSourcePath);
	CatStr(SHADERS_DIR, fragmentFileName, FragmentSourcePath);

	CompileProgram(gameMemory);
}


void Shader::CompileProgram(GameMemory* gameMemory)
{
	Assert(*VertexSourcePath && *FragmentSourcePath);

	FileResult vertexSourceFile = gameMemory->ReadFile(VertexSourcePath);
	FileResult fragmentSourceFile = gameMemory->ReadFile(FragmentSourcePath);
	VertexLastWriteTime = gameMemory->GetLastWriteTime(VertexSourcePath);
	FragmentLastWriteTime = gameMemory->GetLastWriteTime(FragmentSourcePath);

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

	gameMemory->FreeFile(vertexSourceFile.Contents);
	gameMemory->FreeFile(fragmentSourceFile.Contents);

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
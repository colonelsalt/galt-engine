#include "Shader.h"

static constexpr char* SHADERS_DIR = "src/shaders/";

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

static uint32_t ShaderCompileProgram(const char* vertexFileName,
                                     const char* fragmentFileName,
                                     GameMemory* gameMemory)
{
	char* vertexPath = CatStr(SHADERS_DIR, vertexFileName, gameMemory);
	char* fragmentPath = CatStr(SHADERS_DIR, fragmentFileName, gameMemory);

	FileResult vertexSourceFile = gameMemory->ReadFile(vertexPath);
	FileResult fragmentSourceFile = gameMemory->ReadFile(fragmentPath);

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

	return shaderProgramId;
}

static void ShaderDeleteProgram(Shader shader)
{
	glDeleteProgram(shader.RendererId);
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
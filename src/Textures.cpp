#include "Textures.h"

#include <stb_image.h>
#include <glad/glad.h>

constexpr char* DEFAULT_DIRECTORY = "assets/textures/";

uint32_t LoadTexture(char* fileName, GameMemory* memory)
{
	TextureParams params = {};
	params.FileName = fileName;
	params.FlipVertically = true;
	params.UseGammaCorrection = false;
	params.WrappingOption = GL_REPEAT;

	return LoadTexture(&params, memory);
}

uint32_t LoadTexture(TextureParams* texParams, GameMemory* memory)
{
	Assert(texParams->FileName);

	if (!texParams->DirectoryPath)
	{
		texParams->DirectoryPath = DEFAULT_DIRECTORY;
	}

	char path[PATH_MAX];
	CatStr(texParams->DirectoryPath, texParams->FileName, path);

	stbi_set_flip_vertically_on_load(texParams->FlipVertically);
	int width, height, numChannels;
	unsigned char* data = stbi_load(path, &width, &height, &numChannels, 0);
	Assert(data);
	
	uint32_t textureId;
	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_2D, textureId);

	GLenum imageFormat;
	GLint internalFormat;
	if (numChannels == 1)
	{
		imageFormat = GL_RED;
		internalFormat = GL_RED;
	}
	else if (numChannels == 3)
	{
		imageFormat = GL_RGB;
		internalFormat = texParams->UseGammaCorrection ? GL_SRGB : GL_RGB;
	}
	else if (numChannels == 4)
	{
		imageFormat = GL_RGBA;
		internalFormat = texParams->UseGammaCorrection ? GL_SRGB_ALPHA : GL_RGBA;
	}
	else
	{
		Assert(false); // Unknown image format
	}

	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, imageFormat, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, texParams->WrappingOption);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, texParams->WrappingOption);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	stbi_image_free(data);

	return textureId;

}
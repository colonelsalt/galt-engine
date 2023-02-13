#include "Textures.h"

#include <stb_image.h>
#include <glad/glad.h>
#include <assimp/scene.h>

constexpr char* DEFAULT_DIRECTORY = "assets/textures/";

static uint32_t LoadTexture(char* fileName)
{
	TextureParams params = {};
	params.FileName = fileName;
	params.FlipVertically = true;
	params.UseGammaCorrection = false;
	params.WrappingOption = GL_REPEAT;

	return LoadTexture(&params);
}

static uint32_t LoadTexture(TextureParams* texParams)
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

	glTexImage2D(GL_TEXTURE_2D,
	             0,
	             internalFormat,
	             width, height,
	             0,
	             imageFormat,
	             GL_UNSIGNED_BYTE,
	             data);
	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, texParams->WrappingOption);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, texParams->WrappingOption);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	stbi_image_free(data);

	return textureId;

}

uint32_t LoadTextureEmbedded(const aiTexture* texture, TextureParams* texParams)
{
	uint32_t textureId;
	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_2D, textureId);
		
	unsigned char* data = nullptr;

	stbi_set_flip_vertically_on_load(texParams->FlipVertically);
	int width, height, numChannels;

	if (texture->mHeight == 0)
	{
		data = stbi_load_from_memory((unsigned char*)texture->pcData,
		                             texture->mWidth,
		                             &width, &height,
		                             &numChannels,
		                             0);
	}
	else
	{
		data = stbi_load_from_memory((unsigned char*)texture->pcData,
		                             texture->mWidth * texture->mHeight,
		                             &width, &height,
		                             &numChannels,
		                             0);
	}
	Assert(data);

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

	glTexImage2D(GL_TEXTURE_2D,
	             0,
	             internalFormat,
	             width, height,
	             0,
	             imageFormat,
	             GL_UNSIGNED_BYTE,
	             data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, texParams->WrappingOption);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, texParams->WrappingOption);

	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	return textureId;
}

uint32_t LoadCubemap(const char* directoryPath)
{
	uint32_t textureId;
	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureId);

	int width, height, numChannels;
	unsigned char* data;
	constexpr char* faceNames[6] =
	{
		"px.png",
		"nx.png",
		"py.png",
		"ny.png",
		"nz.png",
		"pz.png"
	};
	for (uint32_t i = 0; i < 6; i++)
	{
		char facePath[PATH_MAX];
		CatStr(directoryPath, faceNames[i], facePath);
		stbi_set_flip_vertically_on_load(false);
		data = stbi_load(facePath, &width, &height, &numChannels, 0);
		
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
			internalFormat = GL_RGB;
		}
		else if (numChannels == 4)
		{
			imageFormat = GL_RGBA;
			internalFormat = GL_RGBA;
		}


		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
			             0,
			             internalFormat,
			             width, height,
			             0,
			             imageFormat,
			             GL_UNSIGNED_BYTE,
			             data);
			stbi_image_free(data);
		}
		else
		{
			Assert(false);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureId;
}
#pragma once

struct TextureParams
{
	char* FileName;
	char* DirectoryPath;
	bool FlipVertically;
	bool UseGammaCorrection;
	int WrappingOption;

	inline static int DefaultWrapping()
	{
		return GL_REPEAT;
	}
};

static uint32_t LoadTexture(char* fileName);
static uint32_t LoadTexture(TextureParams* texParams);
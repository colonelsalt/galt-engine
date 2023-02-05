#pragma once

struct TextureParams
{
	char* FileName;
	char* DirectoryPath;
	bool FlipVertically;
	bool UseGammaCorrection;
	int WrappingOption;
};

uint32_t LoadTexture(char* fileName, GameMemory* memory);
uint32_t LoadTexture(TextureParams* texParams, GameMemory* memory);
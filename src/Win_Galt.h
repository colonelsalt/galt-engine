#pragma once

#include "Galt.h"
#include "Util.h"

struct WinGameCode
{
	HMODULE GameCodeDll;
	FILETIME DllLastWriteTime;
	GameUpdateFunc* UpdateAndRender;

	bool IsValid;
};

struct WinExePath
{
	char ExeFullPath[MAX_PATH];
	int LastSlashIndex;
};

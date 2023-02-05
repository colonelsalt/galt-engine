#include <Windows.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Win_Galt.h"

static float s_LastFrameTime;

constexpr int SCREEN_WIDTH = 1280;
constexpr int SCREEN_HEIGHT = 720;

inline FILETIME WinGetLastWriteTime(char* fileName)
{
	FILETIME lastWriteTime = {};
	WIN32_FILE_ATTRIBUTE_DATA data;
	if (GetFileAttributesExA(fileName, GetFileExInfoStandard, &data))
	{
		lastWriteTime = data.ftLastWriteTime;
	}
	return lastWriteTime;
}

int64_t WinGetLastWriteTimeExtern(char* fileName)
{
	ULARGE_INTEGER result = {};
	FILETIME lastWriteTime = WinGetLastWriteTime(fileName);

	result.LowPart = lastWriteTime.dwLowDateTime;
	result.HighPart = lastWriteTime.dwHighDateTime;
	
	return (int64_t)result.QuadPart;
}

static void WinParseExePath(WinExePath* path)
{
	DWORD pathSize = GetModuleFileNameA(0,
	                                    path->ExeFullPath,
	                                    sizeof(path->ExeFullPath));
	for (int i = 0; path->ExeFullPath[i]; i++)
	{
		if (path->ExeFullPath[i] == '\\')
		{
			path->LastSlashIndex = i;
		}
	}
}

static void WinStrCat(char* src1, int length1,
                      char* src2, int length2,
                      char* dest)
{
	for (int i = 0; i < length1; i++)
	{
		*dest++ = src1[i];
	}
	for (int i = 0; i < length2; i++)
	{
		*dest++ = src2[i];
	}
	*dest = 0;
}

static WinGameCode WinLoadGameCode(char* sourceDllName, char* tempDllName)
{
	WinGameCode result = {};

	result.DllLastWriteTime = WinGetLastWriteTime(sourceDllName);
	CopyFile(sourceDllName, tempDllName, FALSE);
	result.GameCodeDll = LoadLibraryA(tempDllName);
	if (result.GameCodeDll)
	{
		result.UpdateAndRender = (GameUpdateFunc*)
			GetProcAddress(result.GameCodeDll, "UpdateAndRender");
		
		result.IsValid = result.UpdateAndRender;
	}
	if (!result.IsValid)
	{
		result.UpdateAndRender = nullptr;
	}
	return result;
}

static void WinUnloadGameCode(WinGameCode* gameCode)
{
	if (gameCode->GameCodeDll)
	{
		FreeLibrary(gameCode->GameCodeDll);
		gameCode->GameCodeDll = 0;
	}
	gameCode->IsValid = false;
	gameCode->UpdateAndRender = nullptr;
}

FileResult WinReadFile(const char* fileName)
{
	FileResult result = {};
	HANDLE fileHandle = CreateFileA(fileName,
	                                GENERIC_READ,
	                                FILE_SHARE_READ,
	                                0,
	                                OPEN_EXISTING,
	                                0, 0);
	Assert(fileHandle != INVALID_HANDLE_VALUE);

	LARGE_INTEGER fileSize;
	Assert(GetFileSizeEx(fileHandle, &fileSize));

	uint32_t fileSize32 = SafeTruncateUInt64(fileSize.QuadPart);

	result.Contents = VirtualAlloc(0,
	                               fileSize32,
	                               MEM_RESERVE | MEM_COMMIT,
	                               PAGE_READWRITE);
	Assert(result.Contents);
	
	DWORD bytesRead;
	BOOL readSuccess = ReadFile(fileHandle,
	                            result.Contents,
	                            fileSize32,
	                            &bytesRead,
	                            0);
	Assert(readSuccess && fileSize32 == bytesRead);

	CloseHandle(fileHandle);

	return result;
}

void WinFreeFileMemory(void* memory)
{
	Assert(memory);
	VirtualFree(memory, 0, MEM_RELEASE);
}

void GetKeyboardInput(GLFWwindow* window, ControllerInput* input)
{
	input->Up = glfwGetKey(window, GLFW_KEY_W);
	input->Down = glfwGetKey(window, GLFW_KEY_S);
	input->Right = glfwGetKey(window, GLFW_KEY_D);
	input->Left = glfwGetKey(window, GLFW_KEY_A);

	input->LeftShoulder = glfwGetKey(window, GLFW_KEY_Q);
	input->RightShoulder = glfwGetKey(window, GLFW_KEY_E);
	input->Back = glfwGetKey(window, GLFW_KEY_ESCAPE);
}


int CALLBACK WinMain(HINSTANCE instance,
                     HINSTANCE prevInstance,
                     LPSTR     commandLine,
                     int       showCode)
{
	// Build path to game code DLL for hot reloading
	constexpr char* gameDllFileName = "Galt.dll";
	constexpr char* tempDllFileName = "Galt_temp.dll";

	WinExePath exePath = {};
	WinParseExePath(&exePath);
	char gameDllFullPath[MAX_PATH];
	WinStrCat(exePath.ExeFullPath, exePath.LastSlashIndex + 1,
	          gameDllFileName, (int)strlen(gameDllFileName),
	          gameDllFullPath);

	char tempDllFullPath[MAX_PATH];
	WinStrCat(exePath.ExeFullPath, exePath.LastSlashIndex + 1,
	          tempDllFileName, (int)strlen(tempDllFileName),
	          tempDllFullPath);

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	
	OutputDebugStringA("sup, homie\n");
	
	// Allocate permanent memory
#if GALT_INTERNAL
	LPVOID baseAddress = (LPVOID)Terabytes(2);
#else
	LPVOID baseAddress = 0;
#endif
	
	GameMemory gameMemory = {};
	gameMemory.PermStorageSize = Megabytes(64);
	gameMemory.TempStorageSize = Gigabytes(1);

	size_t totalSize = gameMemory.PermStorageSize + gameMemory.TempStorageSize;

	void* gameMemoryBlock = VirtualAlloc(baseAddress,
	                                     totalSize,
	                                     MEM_RESERVE | MEM_COMMIT,
	                                     PAGE_READWRITE);
	gameMemory.PermStorage = gameMemoryBlock;
	gameMemory.TempStorage = ((uint8_t*)gameMemory.PermStorage + 
		gameMemory.PermStorageSize);

	gameMemory.ReadFile = WinReadFile;
	gameMemory.FreeFile = WinFreeFileMemory;
	gameMemory.GladLoader = (GLADloadproc)glfwGetProcAddress;
	gameMemory.GetLastWriteTime = WinGetLastWriteTimeExtern;

	Assert(gameMemory.PermStorage);
	Assert(gameMemory.TempStorage);

	// Set up window ---
	int numMonitors;
	GLFWmonitor** monitors = glfwGetMonitors(&numMonitors);
	GLFWmonitor* targetMonitor = monitors[numMonitors - 1];
	const GLFWvidmode* videoMode = glfwGetVideoMode(targetMonitor);

	glfwWindowHint(GLFW_RED_BITS, videoMode->redBits);
	glfwWindowHint(GLFW_GREEN_BITS, videoMode->greenBits);
	glfwWindowHint(GLFW_BLUE_BITS, videoMode->blueBits);
	glfwWindowHint(GLFW_REFRESH_RATE, videoMode->refreshRate);

	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT,
	                                      "Galt engine",
	                                      nullptr,
	                                      nullptr);
	Assert(window);
	glfwMakeContextCurrent(window);

	Assert(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress));

	WinGameCode game = WinLoadGameCode(gameDllFullPath, tempDllFullPath);

	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

	while (!glfwWindowShouldClose(window))
	{
		ControllerInput input = {};
		float timeNow = (float)glfwGetTime();
		input.DeltaTime = timeNow - s_LastFrameTime;
		s_LastFrameTime = timeNow;

		// Check if game code DLL needs reloading
		FILETIME newDllWriteTime = WinGetLastWriteTime(gameDllFullPath);
		if (CompareFileTime(&newDllWriteTime, &game.DllLastWriteTime))
		{
			WinUnloadGameCode(&game);
			game = WinLoadGameCode(gameDllFullPath, tempDllFullPath);
			gameMemory.OpenGlInitialised = false;
		}

		GetKeyboardInput(window, &input);
		if (input.Back)
		{
			break;
		}

		if (game.UpdateAndRender)
		{
			game.UpdateAndRender(&gameMemory, &input);
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}
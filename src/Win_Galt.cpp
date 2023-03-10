#include <Windows.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Win_Galt.h"

static float s_LastFrameTime;
static bool s_PendingScreenResize;

static int s_ScreenWidth = 1280;
static int s_ScreenHeight = 720;

static int s_FrameCounter;
constexpr static int FRAMES_BETWEEN_RELOADS = 10;

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

void WinGetKeyboardInput(GLFWwindow* window, ControllerInput* input)
{
	input->Up = glfwGetKey(window, GLFW_KEY_W);
	input->Down = glfwGetKey(window, GLFW_KEY_S);
	input->Right = glfwGetKey(window, GLFW_KEY_D);
	input->Left = glfwGetKey(window, GLFW_KEY_A);

	input->LeftShoulder = glfwGetKey(window, GLFW_KEY_Q);
	input->RightShoulder = glfwGetKey(window, GLFW_KEY_E);
	input->Back = glfwGetKey(window, GLFW_KEY_ESCAPE);
}

void WinGetMouseInput(GLFWwindow* window, ControllerInput* input)
{
	double mouseX, mouseY;
	glfwGetCursorPos(window, &mouseX, &mouseY);
	input->CameraAxisX = (float)mouseX;
	input->CameraAxisY = (float)mouseY;
}

static constexpr float DEADZONE_EPSILON = 0.2f;

inline static float WinFilterDeadzone(float value)
{
	if (abs(value) < DEADZONE_EPSILON)
	{
		return 0.0f;
	}
	else
	{
		return value;
	}
}

void WinGetJoystickInput(GLFWwindow* window, ControllerInput* input)
{
	GLFWgamepadstate state;
	if (glfwJoystickIsGamepad(GLFW_JOYSTICK_1)
		&& glfwGetGamepadState(GLFW_JOYSTICK_1, &state))
	{
		input->IsAnalogue = true;
		input->MovementAxisX = WinFilterDeadzone(state.axes[GLFW_GAMEPAD_AXIS_LEFT_X]);
		input->MovementAxisY = -WinFilterDeadzone(state.axes[GLFW_GAMEPAD_AXIS_LEFT_Y]);
		input->CameraAxisX = WinFilterDeadzone(state.axes[GLFW_GAMEPAD_AXIS_RIGHT_X]);
		input->CameraAxisY = -WinFilterDeadzone(state.axes[GLFW_GAMEPAD_AXIS_RIGHT_Y]);

		input->LeftTrigger = state.axes[GLFW_GAMEPAD_AXIS_LEFT_TRIGGER];
		input->LeftTrigger = state.axes[GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER];

		input->Up = state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_UP];;
		input->Down = state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_DOWN];
		input->Left = state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_LEFT];
		input->Right = state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_RIGHT];

		input->A = state.buttons[GLFW_GAMEPAD_BUTTON_A];
		input->B = state.buttons[GLFW_GAMEPAD_BUTTON_B];
		input->X = state.buttons[GLFW_GAMEPAD_BUTTON_X];
		input->Y = state.buttons[GLFW_GAMEPAD_BUTTON_Y];

		input->LeftShoulder = state.buttons[GLFW_GAMEPAD_BUTTON_LEFT_BUMPER];
		input->RightShoulder = state.buttons[GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER];
		if (state.buttons[GLFW_GAMEPAD_BUTTON_BACK])
		{
			input->Back = true;
		}
	}
	else
	{
		input->IsAnalogue = false;
	}
}

void WinFrameBufferSizeCallback(GLFWwindow* window, int width, int height)
{
	s_PendingScreenResize = true;
	s_ScreenWidth = width;
	s_ScreenHeight = height;
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
	//glfwWindowHint(GLFW_SAMPLES, 4);
	
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

	gameMemory.PendingScreenResize = true;
	gameMemory.ScreenWidth = s_ScreenWidth;
	gameMemory.ScreenHeight = s_ScreenHeight;

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

	GLFWwindow* window = glfwCreateWindow(s_ScreenWidth, s_ScreenHeight,
	                                      "Galt engine",
	                                      nullptr,
	                                      nullptr);
	Assert(window);
	glfwMakeContextCurrent(window);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	if (glfwRawMouseMotionSupported())
	{
		glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
	}
	glfwSetFramebufferSizeCallback(window, WinFrameBufferSizeCallback);
	glfwMaximizeWindow(window);

	WinGameCode game = WinLoadGameCode(gameDllFullPath, tempDllFullPath);

	ControllerInput input[2] = {0};
	ControllerInput* newInput = &input[0];
	ControllerInput* oldInput = &input[1];

	// Pre-fill current mouse position to prevent camera jump on first frame
	WinGetMouseInput(window, newInput);
	WinGetMouseInput(window, oldInput);

	newInput->LastInput = oldInput;

	while (!glfwWindowShouldClose(window))
	{
		float timeNow = (float)glfwGetTime();
		newInput->DeltaTime = (timeNow - s_LastFrameTime) * DELTA_TIME_SCALE;
		s_LastFrameTime = timeNow;

		char latencyStr[128];
		float ms = newInput->DeltaTime * 1'000;
		int fps = RoundToInt(1.0f / newInput->DeltaTime);
		//sprintf(latencyStr, "%.2fms; %dFPS\n", ms, fps);
		//OutputDebugStringA(latencyStr);


		if (s_FrameCounter++ % FRAMES_BETWEEN_RELOADS == 0)
		{
			// Check if game code DLL needs reloading
			FILETIME newDllWriteTime = WinGetLastWriteTime(gameDllFullPath);
			if (CompareFileTime(&newDllWriteTime, &game.DllLastWriteTime) == 1)
			{
				WinUnloadGameCode(&game);
				game = WinLoadGameCode(gameDllFullPath, tempDllFullPath);
				gameMemory.OpenGlInitialised = false;
			}
		}

		WinGetKeyboardInput(window, newInput);
		WinGetMouseInput(window, newInput);
		WinGetJoystickInput(window, newInput);
		if (newInput->Back)
		{
			break;
		}

		if (s_PendingScreenResize)
		{
			gameMemory.ScreenWidth = s_ScreenWidth;
			gameMemory.ScreenHeight = s_ScreenHeight;
			gameMemory.PendingScreenResize = true;
			s_PendingScreenResize = false;
		}

		gameMemory.FrameCounter = s_FrameCounter;
		if (game.UpdateAndRender)
		{
			game.UpdateAndRender(&gameMemory, newInput);
		}

		ControllerInput* temp = newInput;
		newInput = oldInput;
		oldInput = temp;

		newInput->LastInput = oldInput;

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}
#include <Windows.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Galt.cpp"

constexpr int SCREEN_WIDTH = 1280;
constexpr int SCREEN_HEIGHT = 720;

void GetKeyboardInput(GLFWwindow* window, ControllerInput* input)
{
	input->Up = glfwGetKey(window, GLFW_KEY_W);
	input->Down = glfwGetKey(window, GLFW_KEY_S);
	input->Right = glfwGetKey(window, GLFW_KEY_D);
	input->Left = glfwGetKey(window, GLFW_KEY_A);

	input->Back = glfwGetKey(window, GLFW_KEY_ESCAPE);
}


int CALLBACK WinMain(HINSTANCE instance,
                     HINSTANCE prevInstance,
                     LPSTR     commandLine,
                     int       showCode)
{
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

	size_t totalSize = gameMemory.PermStorageSize;

	gameMemory.PermStorage = VirtualAlloc(baseAddress,
	                                      totalSize,
	                                      MEM_RESERVE | MEM_COMMIT,
	                                      PAGE_READWRITE);

	Assert(gameMemory.PermStorage);

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

	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);


	while (!glfwWindowShouldClose(window))
	{
		ControllerInput input = {};
		GetKeyboardInput(window, &input);
		if (input.Back)
		{
			break;
		}

		UpdateAndRender(&gameMemory, &input);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}
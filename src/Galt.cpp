#include <Windows.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define Assert(x) { if (!(x)) __debugbreak(); }

constexpr int SCREEN_WIDTH = 1920;
constexpr int SCREEN_HEIGHT = 1080;

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
	
	GLFWwindow* window = glfwCreateWindow(1920, 1080,
	                                      "Galt engine",
	                                      nullptr, nullptr);
	Assert(window);
	glfwMakeContextCurrent(window);

	Assert(FreeConsole());

	Assert(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress));

	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	glClearColor(1.0f, 1.0f, 0.0f, 1.0f);

	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}
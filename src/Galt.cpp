#include "Galt.h"

#include "glad/glad.c"
#include "Shader.cpp"
#include "Primitives.cpp"
#include "Camera.cpp"
#include "Textures.cpp"
#include "Transform.cpp"
#include "Mesh.cpp"
#include "Entity.cpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

extern "C" void GAME_API UpdateAndRender(GameMemory* memory, ControllerInput* input)
{
	Assert(sizeof(GameState) <= memory->PermStorageSize);
	
	if (!memory->OpenGlInitialised)
	{
		gladLoadGLLoader(memory->GladLoader);
		memory->OpenGlInitialised = true;
	}
	if (memory->PendingScreenResize)
	{
		glViewport(0, 0, memory->ScreenWidth, memory->ScreenHeight);
		memory->PendingScreenResize = false;
	}

	GameState* state = (GameState*)memory->PermStorage;
	if (!memory->IsInitialised)
	{
		glEnable(GL_DEPTH_TEST);

		Shader* primitiveShader = &state->PrimitiveShader;
		primitiveShader->CompileProgram("PrimitiveVert.glsl",
		                                "PrimitiveFrag.glsl",
		                                memory);
		state->FpsCamera = CreateCamera();
		CreatePlane(&state->Plane, primitiveShader, "wood.png", memory);
		CreateCube(&state->Cube, primitiveShader, "container.png", memory);
		state->Cube.Transform.Translation()->y = 0.5001f;

		LoadMesh("Lamp/Lamp.fbx", &state->Lamp, memory);
		glm::vec3* lampPos = state->Lamp.Transform.Translation();
		*lampPos = { 3.0f, -0.5f, 1.0f };
		state->Lamp.Transform.SetRotation(0.0f, -90.0f, 0.0f);



		memory->IsInitialised = true;
	}

	for (int i = 0; i < ArrayCount(state->Shaders); i++)
	{
		Shader* shader = &state->Shaders[i];
		shader->ReloadIfNeeded(memory);
	}

	state->FpsCamera.Update(input);

	// Rendering ---
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	state->PrimitiveShader.Bind();

	state->PrimitiveShader.SetMat4("u_Projection", state->FpsCamera.Projection);
	state->PrimitiveShader.SetMat4("u_View", state->FpsCamera.View);


	state->Plane.Draw(&state->PrimitiveShader);

	state->Cube.Draw(&state->PrimitiveShader);

	state->Lamp.Draw(&state->PrimitiveShader);

	glBindVertexArray(0);

}
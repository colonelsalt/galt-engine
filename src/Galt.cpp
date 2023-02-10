#include "Galt.h"

#include "glad/glad.c"

// ---
// Globals ---
// ---
static GameMemory* g_Memory;
static EntityMaster* g_EntityMaster;

#include "Shader.cpp"
#include "Primitives.cpp"
#include "Camera.cpp"
#include "Textures.cpp"
#include "Transform.cpp"
#include "Mesh.cpp"
#include "Entity.cpp"
#include "Renderer.cpp"
#include "EntityMaster.cpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

extern "C" void GAME_API UpdateAndRender(GameMemory* memory, ControllerInput* input)
{
	Assert(sizeof(GameState) <= memory->PermStorageSize);
	
	GameState* state = (GameState*)memory->PermStorage;
	if (!memory->OpenGlInitialised)
	{
		gladLoadGLLoader(memory->GladLoader);
		g_Memory = memory;
		g_EntityMaster = &state->EntityMasterInstance;
		memory->OpenGlInitialised = true;
	}
	if (memory->PendingScreenResize)
	{
		glViewport(0, 0, memory->ScreenWidth, memory->ScreenHeight);
		memory->PendingScreenResize = false;
	}

	if (!memory->IsInitialised)
	{
		glEnable(GL_DEPTH_TEST);
		state->EntityMasterInstance.InitComponents();

		Shader* primitiveShader = &state->PrimitiveShader;
		primitiveShader->CompileProgram("PrimitiveVert.glsl",
		                                "PrimitiveFrag.glsl");
		state->FpsCamera = CreateCamera();
		state->Plane = CreatePlane(primitiveShader, "wood.png");
		state->Cube = CreateCube(primitiveShader, "container.png");
		NameTag* cubeName = state->Cube.AddComponent<NameTag>();
		cubeName->Init("Box");

		Transform* cubeTransform = state->Cube.GetComponent<Transform>();
		cubeTransform->Translation()->y = 0.5001f;

		state->Lamp = LoadMesh("Lamp/Lamp.fbx");
		glm::vec3* lampPos = state->Lamp.GetComponent<Transform>()->Translation();
		*lampPos = { 3.0f, -0.5f, 1.0f };
		Transform* lampTransform = state->Lamp.GetComponent<Transform>();
		lampTransform->SetRotation(0.0f, -90.0f, 0.0f);


		memory->IsInitialised = true;
	}

	for (int i = 0; i < ArrayCount(state->Shaders); i++)
	{
		Shader* shader = &state->Shaders[i];
		shader->ReloadIfNeeded();
	}

	state->FpsCamera.Update(input);

	// Rendering ---
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	state->PrimitiveShader.Bind();
	
	glm::vec3* lampPos = state->Lamp.GetComponent<Transform>()->Translation();
	//lampPos->y += 0.001f;

	state->PrimitiveShader.SetMat4("u_Projection", state->FpsCamera.Projection);
	state->PrimitiveShader.SetMat4("u_View", state->FpsCamera.View);

	NameTag* cubeTag = state->Cube.GetComponent<NameTag>();

	RenderScene(&state->PrimitiveShader);

	glBindVertexArray(0);

}
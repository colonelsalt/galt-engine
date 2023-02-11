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
		glEnable(GL_CULL_FACE);

		state->EntityMasterInstance.InitComponents();

		Shader* primitiveShader = &state->PrimitiveShader;
		primitiveShader->CompileProgram("PrimitiveVert.glsl",
		                                "PrimitiveFrag.glsl");
		Shader* basicPhongShader = &state->BasicPhongShader;
		basicPhongShader->CompileProgram("BasicPhongVert.glsl",
		                                "BasicPhongFrag.glsl");
		Shader* flatColourShader = &state->FlatColourShader;
		flatColourShader->CompileProgram("BasicVert.glsl",
		                                 "FlatColourFrag.glsl");

		state->FpsCamera = CreateCamera();
		state->Plane = CreatePlane("wood.png");
		state->Plane.GetComponent<Primitive>()->p_Shader = basicPhongShader;

		state->Cube = CreateCube("container.png", "container_specular.png");
		state->Cube.GetComponent<Primitive>()->p_Shader = basicPhongShader;

		state->Cube.Trans()->Position()->y = 0.5001f;

		state->Lamp = LoadMesh("Lamp/Lamp.fbx");
		SetShaderInHierarchy(state->Lamp.Trans(), basicPhongShader);

		glm::vec3* lampPos = state->Lamp.Trans()->Position();
		*lampPos = { 3.0f, -0.5f, 1.0f };
		Transform* lampTransform = state->Lamp.Trans();
		lampTransform->SetRotation(0.0f, -90.0f, 0.0f);

		state->PointLight = g_EntityMaster->CreateEntity("DirLight");
		Light* lightLight = state->PointLight.AddComponent<Light>();
		Primitive* lightCube = state->PointLight.AddComponent<Primitive>();
		PopulateCube(lightCube, "white.png");
		lightCube->p_Shader = flatColourShader;

		lightLight->Type = LightType::POINT;

		RenderSetup(state);
		memory->IsInitialised = true;
	}

	for (int i = 0; i < ArrayCount(state->Shaders); i++)
	{
		Shader* shader = &state->Shaders[i];
		shader->ReloadIfNeeded();
	}

	state->FpsCamera.Update(input);
	glm::vec3* lampPos = state->Lamp.Trans()->Position();

	Transform* lightTransform = state->PointLight.Trans();
	glm::vec3* lightPos = lightTransform->Position();
	
	lightTransform->SetScale( { 0.25f, 0.25f, 0.25f });

	*lightPos = { 1.0f, 3.0f, 0.0f };


	Light* lightLight = state->PointLight.GetComponent<Light>();
	lightLight->Colour = { 1.0f, 1.0f, 1.0f };

	RenderScene(state);
}
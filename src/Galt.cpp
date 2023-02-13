#include "Galt.h"

#include "glad/glad.c"

// ---
// Globals ---
// ---
static GameMemory* g_Memory;
static EntityMaster* g_EntityMaster;

static constexpr int FRAMES_BETWEEN_RELOADS = 10;

#include "Shader.cpp"
#include "Primitives.cpp"
#include "Camera.cpp"
#include "Textures.cpp"
#include "HashMap.cpp"
#include "Component.cpp"
#include "Transform.cpp"
#include "Mesh.cpp"
#include "Entity.cpp"
#include "Renderer.cpp"
#include "EntityMaster.cpp"
#include "AnimationClip.cpp"
#include "Animator.cpp"
#include "Player.cpp"

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
		glDepthFunc(GL_LEQUAL);
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
		Shader* meshShader = &state->MeshShader;
		meshShader->CompileProgram("MeshVert.glsl", "MeshFrag.glsl");

		Shader* animShader = &state->AnimShader;
		animShader->CompileProgram("SkinnedMeshVert.glsl",
		                           "MeshFrag.glsl");

		Shader* skyboxShader = &state->SkyboxShader;
		skyboxShader->CompileProgram("SkyboxVert.glsl", "SkyboxFrag.glsl");

		state->FpsCamera = CreateCamera();
		state->Plane = CreatePlane("Grass_diffuse.jpg");
		state->Plane.GetComponent<Primitive>()->p_Shader = basicPhongShader;

		state->Cube = CreateCube("container.png", "container_specular.png");
		state->Cube.GetComponent<Primitive>()->p_Shader = basicPhongShader;

		state->Cube.Trans()->Position()->y = 0.5001f;

		state->Player = LoadMesh("Boss/The Boss.fbx");
		state->IdleClip = ParseAnimationClip("Boss/idle.fbx");
		state->IdleClip->ShouldLoop = true;

		Animator* playerAnimator = state->Player.AddComponent<Animator>();
		playerAnimator->Init(state->IdleClip);

		Player* playerPlayer = state->Player.AddComponent<Player>();
		playerPlayer->Init();

		Transform* playerTransform = state->Player.Trans();
		playerTransform->SetScale( { 0.15f, 0.15f, 0.15f });

		glm::vec3* playerPos = playerTransform->Position();
		*playerPos = { -2.0f, -0.45f, -1.0f };

		SetShaderInHierarchy(state->Player.Trans(), animShader);

		state->PointLight = g_EntityMaster->CreateEntity("DirLight");
		Light* lightLight = state->PointLight.AddComponent<Light>();
		Primitive* lightCube = state->PointLight.AddComponent<Primitive>();
		PopulateCube(lightCube, "white.png");
		lightCube->p_Shader = flatColourShader;

		lightLight->Type = LightType::POINT;

		state->Skybox = CreateCube();
		Primitive* cubePrimitive = state->Skybox.GetComponent<Primitive>();
		cubePrimitive->SkyboxTextureId = LoadCubemap("assets/textures/skybox/");
		cubePrimitive->p_Shader = &state->SkyboxShader;

		RenderSetup(state);
		memory->IsInitialised = true;
	}

	if (memory->FrameCounter % FRAMES_BETWEEN_RELOADS == 0)
	{
		for (int i = 0; i < ArrayCount(state->Shaders); i++)
		{
			Shader* shader = &state->Shaders[i];
			shader->ReloadIfNeeded();
		}
	}

	Transform* lightTransform = state->PointLight.Trans();
	glm::vec3* lightPos = lightTransform->Position();
	
	lightTransform->SetScale( { 0.25f, 0.25f, 0.25f });

	*lightPos = { 1.0f, 3.0f, 0.0f };

	Light* lightLight = state->PointLight.GetComponent<Light>();
	lightLight->Colour = { 1.0f, 1.0f, 1.0f };

	state->FpsCamera.StickSensitivity = 0.017f;
	
	// Update camera
	state->FpsCamera.Update(input);

	// Update animators
	uint32_t numAnimators = g_EntityMaster->NumComponents(ComponentType::ANIMATOR);
	Animator* animators = g_EntityMaster->GetAllComponents<Animator>();
	Assert(numAnimators == 1);

	Player* player = state->Player.GetComponent<Player>();
	player->Update(input, &state->FpsCamera);

	// Update root transforms (and recursively all their children)
	uint32_t numTransforms = g_EntityMaster->NumComponents(ComponentType::TRANSFORM);
	Transform* transforms = g_EntityMaster->GetAllComponents<Transform>();
	for (uint32_t i = 0; i < numTransforms; i++)
	{
		Transform* transform = &transforms[i];
		Assert(transform);
		if (!transform->p_Parent)
		{
			transform->Update();
		}
	}

	Animator* playerAnimator = &animators[0];
	playerAnimator->Update(input->DeltaTime);

	RenderScene(state);
}
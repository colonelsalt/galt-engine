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
#include "AnimationStates.cpp"
#include "Lights.cpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

static void UpdateTransforms()
{
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
}

static void ConstructPlayerBlendTree(Animator* playerAnimator)
{
	TranslationConstraints constrainZ = {};
	constrainZ.Z = true;

	AnimationClip* idleClip = ParseAnimationClip("Boss/idle.fbx", "IdleClip");
	AnimationClip* walkClip = ParseAnimationClip("Boss/walking.fbx", "WalkingClip");
	walkClip->SetConstraints(constrainZ);

	AnimationClip* runClip = ParseAnimationClip("Boss/running.fbx", "RunningClip");
	runClip->SetConstraints(constrainZ);

	AnimationClip* jumpClip = ParseAnimationClip("Boss/jumping up.fbx", "JumpClip");

	AnimationClip* fallClip = ParseAnimationClip("Boss/falling idle.fbx", "FallingClip");

	AnimationClip* landClip = ParseAnimationClip("Boss/hard landing.fbx", "LandingClip");
	landClip->Constraints = constrainZ;

	AnimationClip* rollClip = ParseAnimationClip("Boss/falling to roll.fbx", "RollingClip");
	rollClip->LocalDuration -= 15;
	rollClip->SetConstraints(constrainZ);

	AnimationClip* locomotionClip = MakeCompositeAnimationClip(walkClip,
	                                                           runClip,
	                                                           "LocomotionClip");

	AnimationState* idleState = CreateAnimationState(idleClip, "IdleState");
	idleState->ShouldLoop = true;

	AnimationState* locomotionState = CreateAnimationState(locomotionClip,
	                                                       "LocomotionState");
	locomotionState->ShouldLoop = true;
	locomotionState->BlendVariable.VarId = PlayerAnimVars::MOVE_SPEED;

	AnimationState* jumpState = CreateAnimationState(jumpClip, "JumpState");
	AnimationState* fallState = CreateAnimationState(fallClip, "FallingState");
	fallState->ShouldLoop = true;
	AnimationState* landState = CreateAnimationState(landClip, "LandingState");
	AnimationState* rollState = CreateAnimationState(rollClip, "RollingState");

	Transition* idleToLocomotion = CreateTransition(idleState,
	                                                locomotionState,
	                                                0.2f,
	                                                "IdleToLocomotion");
	idleState->ap_OnTriggerTransitions[PlayerAnimTriggers::MOVE] = idleToLocomotion;

	Transition* locomotionToIdle = CreateTransition(locomotionState,
	                                                idleState,
	                                                0.3f,
	                                                "LocomotionToIdle");
	locomotionState->ap_OnTriggerTransitions[PlayerAnimTriggers::IDLE] = locomotionToIdle;

	Transition* idleToJump = CreateTransition(idleState,
	                                          jumpState,
	                                          0.2f,
	                                          "IdleToJump");
	idleState->ap_OnTriggerTransitions[PlayerAnimTriggers::JUMP] = idleToJump;
	
	Transition* locomotionToJump = CreateTransition(locomotionState,
	                                                jumpState,
	                                                0.2f,
	                                                "LocomotionToJump");
	locomotionState->ap_OnTriggerTransitions[PlayerAnimTriggers::JUMP] = locomotionToJump;

	Transition* jumpToFall = CreateTransition(jumpState,
	                                          fallState,
	                                          0.2f,
	                                          "JumpToFall");
	jumpState->p_OnCompleteTransition = jumpToFall;

	Transition* fallToLand = CreateTransition(fallState,
	                                          landState,
	                                          0.1f,
	                                          "FallToLand");
	fallState->ap_OnTriggerTransitions[PlayerAnimTriggers::LAND] = fallToLand;

	Transition* landToIdle = CreateTransition(landState,
	                                          idleState,
	                                          0.3f,
	                                          "LandToIdle");
	landState->p_OnCompleteTransition = landToIdle;

	Transition* fallToRoll = CreateTransition(fallState,
	                                          rollState,
	                                          0.2f,
	                                          "FallToRoll");
	fallState->ap_OnTriggerTransitions[PlayerAnimTriggers::ROLL] = fallToRoll;

	Transition* rollToLocomotion = CreateTransition(rollState,
	                                                locomotionState,
	                                                0.5f,
	                                                "RollToLocomotion");

	Transition* rollToIdle = CreateTransition(rollState,
	                                          idleState,
	                                          0.2f,
	                                          "RollToIdle");
	rollState->p_OnCompleteTransition = rollToIdle;

	playerAnimator->p_CurrentState = idleState;
}

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
		Shader* primitiveShadowShader = &state->PrimitiveShadowShader;
		primitiveShadowShader->CompileProgram("BasicPhongVert.glsl",
		                                      "EmptyFrag.glsl");

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

		Shader* meshShadowShader = &state->MeshShadowShader;
		meshShadowShader->CompileProgram("SkinnedMeshVert.glsl", "EmptyFrag.glsl");

		state->FpsCamera = CreateCamera();
		state->Plane = CreatePlane("Plane", "Grass_diffuse.jpg");
		state->Plane.GetComponent<Primitive>()->p_Shader = basicPhongShader;

		state->Cube = CreateCube("Box", "container.png", "container_specular.png");
		state->Cube.GetComponent<Primitive>()->p_Shader = basicPhongShader;

		state->Cube.Trans()->Position()->y = 0.5001f;

		state->Player = LoadMesh("Boss/The Boss.fbx");

		Animator* playerAnimator = state->Player.AddComponent<Animator>();
		playerAnimator->Init();
		ConstructPlayerBlendTree(playerAnimator);

		glm::vec3* playerPos = state->Player.Trans()->Position();
		*playerPos = { -2.0f, -0.45f, -1.0f };
		Player* playerPlayer = state->Player.AddComponent<Player>();
		playerPlayer->Init();

		Transform* playerTransform = state->Player.Trans();
		playerTransform->SetScale( { 0.15f, 0.15f, 0.15f });

		SetShaderInHierarchy(playerTransform, animShader);
		SetShaderInHierarchy(playerTransform, meshShadowShader, RenderPass::SHADOWS);

		state->PointLight = g_EntityMaster->CreateEntity("DirLight");
		Light* lightLight = state->PointLight.AddComponent<Light>();
		Primitive* lightCube = state->PointLight.AddComponent<Primitive>();
		PopulateCube(lightCube, "white.png");
		lightCube->p_Shader = flatColourShader;

		lightLight->Type = LightType::POINT;

		state->Skybox = CreateCube("Skybox");
		Primitive* cubePrimitive = state->Skybox.GetComponent<Primitive>();
		cubePrimitive->SkyboxTextureId = LoadCubemap("assets/textures/skybox/");
		cubePrimitive->p_Shader = &state->SkyboxShader;

		UpdateTransforms();

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

	*lightPos = { 3.0f, 7.0f, 0.0f };

	Light* lightLight = state->PointLight.GetComponent<Light>();
	lightLight->Colour = { 1.0f, 1.0f, 1.0f };

	// Update camera
	state->FpsCamera.Update(input);

	// Update animators
	uint32_t numAnimators = g_EntityMaster->NumComponents(ComponentType::ANIMATOR);
	Animator* animators = g_EntityMaster->GetAllComponents<Animator>();
	Assert(numAnimators == 1);

	Player* player = state->Player.GetComponent<Player>();
	player->Update(input, &state->FpsCamera);

	Animator* playerAnimator = &animators[0];
	playerAnimator->Update(input->DeltaTime);

	RenderScene(state);
}
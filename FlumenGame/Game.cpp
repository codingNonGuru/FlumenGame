#include "FlumenEngine/Core/Engine.hpp"
#include "FlumenEngine/Render/Model.hpp"
#include "FlumenEngine/Render/ModelManager.hpp"
#include "FlumenEngine/Render/ShaderManager.hpp"
#include "FlumenEngine/Render/MeshManager.hpp"
#include "FlumenEngine/Render/Mesh.hpp"
#include "FlumenEngine/Render/Shader.hpp"
#include "FlumenEngine/Core/State.hpp"
#include "FlumenEngine/Core/StateManager.hpp"
#include "FlumenEngine/Render/Camera.hpp"
#include "FlumenEngine/Render/RenderManager.hpp"

#include "FlumenGame/Game.hpp"
#include "Interface/InterfaceBuilder.hpp"
#include "SceneBuilder.hpp"
#include "FlumenGame/Filter/FilterBuilder.hpp"
#include "FlumenGame/Types.hpp"
#include "FlumenGame/States/PregameState.hpp"
#include "FlumenGame/States/WorldState.hpp"

Game* Game::instance_ = Game::Register();

Delegate Game::OnStartGame_ = Delegate();

void Game::Initialize()
{
	auto screen = Engine::GetScreen();

	Camera* camera = nullptr;

	camera = new Camera(screen, Position3(0.0f, 0.0f, 0.0f), 0.7f, 0.0f, 3.0f);
	RenderManager::AddCamera(Cameras::PREGAME, camera);

	camera = new Camera(screen, Position3(0.0f, 0.0f, 0.0f), 0.5f, 0.0f, 3.0f);
	RenderManager::AddCamera(Cameras::WORLD, camera);

	auto cubeMesh = MeshManager::GetMeshes().Get("Sphere");
	auto genericShader = ShaderManager::GetShader("Generic");

	if(cubeMesh != nullptr)
	{
		auto cubeModel = new Model(cubeMesh, genericShader);
		ModelManager::AddModel(cubeModel, "Building");
	}

	SceneBuilder::GenerateScenes();

	FilterBuilder::GenerateFilters();

	InterfaceBuilder::GenerateInterface();

	StateManager::Register(new State(), States::INTRO);
	StateManager::Register(new PregameState(), States::PREGAME);
	StateManager::Register(new WorldState(), States::WORLD);
}

void Game::StartGame()
{
	OnStartGame_.Invoke();

	StateManager::Enter(States::WORLD);
}

Game* Game::Register()
{
	Engine::OnInitialize_.Add(&Game::Initialize);

	Engine::OnGameLoopStart_.Add(&Game::HandleGameLoopStart);

	return new Game();
}

void Game::HandleGameLoopStart()
{
	StateManager::Enter(States::PREGAME);
}

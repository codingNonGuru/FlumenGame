#include "FlumenEngine/Core/SceneManager.hpp"

#include "SceneBuilder.hpp"
#include "FlumenGame/Scenes/WorldScene.hpp"
#include "FlumenGame/Scenes/PregameScene.hpp"
#include "FlumenGame/Types.hpp"

void SceneBuilder::GenerateScenes()
{
	SceneManager::AddScene(new PregameScene(), Scenes::PREGAME);

	SceneManager::AddScene(new WorldScene(), Scenes::WORLD);
}


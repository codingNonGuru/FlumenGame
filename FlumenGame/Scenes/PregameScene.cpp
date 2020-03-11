#include "SDL2/SDL.h"

#include "FlumenEngine/Render/Model.hpp"
#include "FlumenEngine/Render/ModelManager.hpp"
#include "FlumenEngine/Render/RenderManager.hpp"
#include "FlumenEngine/Render/Camera.hpp"
#include "FlumenEngine/Core/InputHandler.hpp"

#include "FlumenGame/Scenes/PregameScene.hpp"
#include "FlumenGame/Types.hpp"

PregameScene::PregameScene()
{
	camera_ = RenderManager::GetCamera(Cameras::PREGAME);
}

void PregameScene::Update()
{
	if(InputHandler::IsPressed(SDL_SCANCODE_A))
	{
		camera_->Spin(0.01f);
	}
	else if(InputHandler::IsPressed(SDL_SCANCODE_D))
	{
		camera_->Spin(-0.01f);
	}
}

void PregameScene::Render()
{
	RenderManager::EnableDepthTesting();

	auto cubeModel = ModelManager::GetModel("Building");
	cubeModel->Render(camera_, nullptr);
}

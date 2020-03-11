#include "FlumenEngine/Interface/Interface.hpp"
#include "FlumenEngine/Interface/Element.hpp"
#include "FlumenEngine/Core/TaskManager.hpp"
#include "FlumenEngine/Core/SceneManager.hpp"
#include "FlumenEngine/Core/Scene.hpp"

#include "FlumenGame/States/PregameState.hpp"
#include "FlumenGame/Types.hpp"

PregameState::PregameState()
{
	pregameScene_ = SceneManager::Get(Scenes::PREGAME);

	mainMenu_ = Interface::GetElement(Elements::MAIN_MENU);

	newWorldMenu_ = Interface::GetElement(Elements::NEW_WORLD_MENU);

	worldPreviewPanel_ = Interface::GetElement(Elements::WORLD_PREVIEW_PANEL);

	mainMenuOpenDelay_ = 1.0f;
}

void PregameState::HandleEnter()
{
	if(mainMenu_ != nullptr)
	{
		TaskManager::Add()->Initialize(mainMenu_, &Element::Open, mainMenuOpenDelay_);
	}

	if(pregameScene_ != nullptr)
	{
		pregameScene_->Enable();
	}
}

void PregameState::HandleExit()
{
	if(newWorldMenu_ != nullptr)
	{
		newWorldMenu_->Close();
	}

	if(worldPreviewPanel_ != nullptr)
	{
		worldPreviewPanel_->Close();
	}

	if(pregameScene_ != nullptr)
	{
		pregameScene_->Disable();
	}
}




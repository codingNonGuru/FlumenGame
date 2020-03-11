#include "FlumenEngine/Core/Scene.hpp"
#include "FlumenEngine/Core/SceneManager.hpp"
#include "FlumenEngine/Core/TaskManager.hpp"
#include "FlumenEngine/Interface/Interface.hpp"
#include "FlumenEngine/Interface/Element.hpp"

#include "FlumenGame/States/WorldState.hpp"
#include "FlumenGame/Types.hpp"

WorldState::WorldState()
{
	worldScene_ = SceneManager::Get(Scenes::WORLD);

	topBar_ = Interface::GetElement(Elements::TOP_BAR);

	topPanel_ = Interface::GetElement(Elements::TOP_PANEL);
}

void WorldState::HandleEnter()
{
	TaskManager::Add()->Initialize(this, &WorldState::HandleSceneEnable, 2.0f);
}

void WorldState::HandleExit()
{
	if(worldScene_ != nullptr)
	{
		worldScene_->Disable();
	}

	topBar_->Close();

	topPanel_->Close();
}

void WorldState::HandleSceneEnable()
{
	if(worldScene_ != nullptr)
	{
		worldScene_->Enable();
	}

	topBar_->Open();

	topPanel_->Open();
}

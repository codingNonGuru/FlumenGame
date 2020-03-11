#include "MainMenu.hpp"
#include "FlumenEngine/Interface/Interface.hpp"
#include "FlumenEngine/Animation/Animator.hpp"
#include "FlumenEngine/Animation/Animation.hpp"
#include "FlumenEngine/Animation/AnimationEvent.hpp"
#include "FlumenCore/Delegate/Delegate.hpp"
#include "FlumenEngine/Render/Filter.hpp"
#include "FlumenEngine/Render/FilterManager.hpp"

#include "FlumenGame/Types.hpp"

void MainMenu::HandleInitialize()
{
	saturateFilter_ = FilterManager::GetFilter("Saturate");
	blurFilter_ = FilterManager::GetFilter("Blur");

	newGameMenu_ = Interface::GetElement(Elements::NEW_GAME_MENU);

	if(closeEvent_ != nullptr)
	{
		closeEvent_->GetActions().Add(this, &MainMenu::OpenNewGameMenu);
	}
}

void MainMenu::HandleOpen()
{
	if(saturateFilter_)
	{
		saturateFilter_->Open();
	}

	if(blurFilter_)
	{
		blurFilter_->Open();
	}
}

void MainMenu::HandleClose()
{
	if(saturateFilter_)
	{
		saturateFilter_->Close();
	}

	if(blurFilter_)
	{
		blurFilter_->Close();
	}
}

void MainMenu::OpenNewGameMenu()
{
	if(newGameMenu_)
	{
		newGameMenu_->Open();
	}
}

#include "FlumenEngine/Animation/Animator.hpp"
#include "FlumenEngine/Animation/Animation.hpp"
#include "FlumenEngine/Animation/AnimationEvent.hpp"
#include "FlumenCore/Delegate/Delegate.hpp"
#include "FlumenEngine/Interface/Interface.hpp"
#include "FlumenEngine/Interface/Text.hpp"
#include "FlumenEngine/Core/SceneManager.hpp"

#include "NewWorldMenu.hpp"
#include "FlumenGame/Generators/WorldParameterSet.hpp"
#include "FlumenGame/Generators/WorldGenerator.hpp"
#include "FlumenGame/Scenes/WorldScene.hpp"
#include "FlumenGame/Game.hpp"

void NewWorldMenu::HandleInitialize()
{
	newGameMenu_ = Interface::GetElement(Elements::NEW_GAME_MENU);

	sizeOptionButton_ = GetChild("SizeButton");

	leftScrollButton_ = GetChild("LeftScrollButton");

	rightScrollButton_ = GetChild("RightScrollButton");

	startGameButton_ = GetChild("StartGameButton");

	saveWorldButton_ = GetChild("SaveWorldButton");

	discardButton_ = GetChild("DiscardButton");

	worldPreview_ = Interface::GetElement(Elements::WORLD_PREVIEW_PANEL);

	if(sizeOptionButton_ != nullptr)
	{
		sizeLabel_ = sizeOptionButton_->GetChild("SizeLabel");
	}

	mainScene_ = SceneManager::Get(Scenes::WORLD);

	sizeOption_ = WorldSizeOptions::MEDIUM;

	if(sizeOption_ == WorldSizeOptions::TINY && leftScrollButton_ != nullptr)
	{
		leftScrollButton_->Disable();
	}

	if(startGameButton_ != nullptr)
	{
		auto & clickEvents = startGameButton_->GetClickEvents();
		clickEvents.Add(this, &NewWorldMenu::StartGame);
	}

	UpdateSizeLabel();

	WorldGenerator::OnWorldGenerated_.Add(this, &NewWorldMenu::FinishGeneration);
}

void NewWorldMenu::OpenNewGameMenu()
{
	closeEvent_->GetActions().Add(this, &NewWorldMenu::HandleNewGameMenuOpen);

	Close();
}

void NewWorldMenu::HandleNewGameMenuOpen()
{
    if(newGameMenu_ != nullptr)
    {
    	newGameMenu_->Open();
    }

    if(worldPreview_ != nullptr)
    {
    	worldPreview_->Close();
    }

    closeEvent_->GetActions().Remove(this, &NewWorldMenu::HandleNewGameMenuOpen);
}

void NewWorldMenu::ScrollLeftwards()
{
	if(sizeOption_ == WorldSizeOptions::SMALL)
	{
		leftScrollButton_->Disable();
	}

	if(sizeOption_ == WorldSizeOptions::IMMENSE)
	{
		rightScrollButton_->Enable();
	}

	sizeOption_ = WorldSizeOptions((int)sizeOption_ - 1);

	UpdateSizeLabel();
}

void NewWorldMenu::ScrollRightwards()
{
	if(sizeOption_ == WorldSizeOptions::LARGE)
	{
		rightScrollButton_->Disable();
	}

	if(sizeOption_ == WorldSizeOptions::TINY)
	{
		leftScrollButton_->Enable();
	}

	sizeOption_ = WorldSizeOptions((int)sizeOption_ + 1);

	UpdateSizeLabel();
}

void NewWorldMenu::UpdateSizeLabel()
{
	if(sizeLabel_ == nullptr)
		return;

	switch(sizeOption_)
	{
	case WorldSizeOptions::TINY:
		sizeLabel_->Setup("256 x 256");
		break;
	case WorldSizeOptions::SMALL:
		sizeLabel_->Setup("512 x 512");
		break;
	case WorldSizeOptions::MEDIUM:
		sizeLabel_->Setup("768 x 768");
		break;
	case WorldSizeOptions::LARGE:
		sizeLabel_->Setup("1024 x 1024");
		break;
	case WorldSizeOptions::IMMENSE:
		sizeLabel_->Setup("1536 x 1536");
		break;
	}
}

WorldParameterSet parameterSet;

void NewWorldMenu::GenerateWorld()
{
	auto generateButton = GetChild("GenerateButton");
	if(generateButton != nullptr)
	{
		generateButton->Disable();
	}

	if(sizeOptionButton_ != nullptr)
	{
		sizeOptionButton_->Disable();
	}

	if(leftScrollButton_ != nullptr)
	{
		leftScrollButton_->Disable();
	}

	if(rightScrollButton_ != nullptr)
	{
		rightScrollButton_->Disable();
	}

	if(worldPreview_ != nullptr)
	{
		worldPreview_->Open();
	}

	auto backButton = GetChild("BackButton");
	if(backButton != nullptr)
	{
		backButton->Disable();
	}

	if(mainScene_ != nullptr)
	{
		parameterSet.SizeOption_ = sizeOption_;

		mainScene_->Initialize(parameterSet);
	}
}

void NewWorldMenu::FinishGeneration()
{
	auto backButton = GetChild("BackButton");
	if(backButton != nullptr)
	{
		backButton->Enable();
	}

	if(startGameButton_ != nullptr)
	{
		startGameButton_->Enable();
	}

	if(saveWorldButton_ != nullptr)
	{
		saveWorldButton_->Enable();
	}

	if(discardButton_ != nullptr)
	{
		discardButton_->Enable();
	}
}

void NewWorldMenu::StartGame()
{
	Game::StartGame();
}


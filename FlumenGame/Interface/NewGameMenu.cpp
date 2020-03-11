#include "FlumenEngine/Animation/Animator.hpp"
#include "FlumenEngine/Animation/Animation.hpp"
#include "FlumenEngine/Animation/AnimationEvent.hpp"
#include "FlumenCore/Delegate/Delegate.hpp"
#include "FlumenEngine/Interface/Interface.hpp"

#include "NewGameMenu.hpp"
#include "FlumenGame/Types.hpp"

void NewGameMenu::HandleInitialize()
{
	mainMenu_ = Interface::GetElement("MainMenu");

	newWorldMenu_ = Interface::GetElement(Elements::NEW_WORLD_MENU);
}

void NewGameMenu::OpenNewWorldMenu()
{
	closeEvent_->GetActions().Add(this, &NewGameMenu::HandleNewWorldOpen);

	Close();
}

void NewGameMenu::HandleNewWorldOpen()
{
    if(newWorldMenu_ != nullptr)
    {
    	newWorldMenu_->Open();
    }

    closeEvent_->GetActions().Remove(this, &NewGameMenu::HandleNewWorldOpen);
}

void NewGameMenu::OpenMainMenu()
{
	closeEvent_->GetActions().Add(this, &NewGameMenu::HandleMainMenuOpen);

	Close();
}

void NewGameMenu::HandleMainMenuOpen()
{
    if(mainMenu_ != nullptr)
    {
    	mainMenu_->Open();
    }

    closeEvent_->GetActions().Remove(this, &NewGameMenu::HandleMainMenuOpen);
}

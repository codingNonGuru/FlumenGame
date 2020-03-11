#pragma once

#include "FlumenEngine/Interface/Element.hpp"
#include "FlumenGame/Types.hpp"

class AnimationEvent;
class Text;
class WorldScene;

class NewWorldMenu : public Element
{
	Element* newGameMenu_;

	Element* sizeOptionButton_;

	Element* leftScrollButton_;

	Element* rightScrollButton_;

	Text* sizeLabel_;

	Element* startGameButton_;

	Element* saveWorldButton_;

	Element* discardButton_;

	Element* worldPreview_;

	WorldScene* mainScene_;

	WorldSizeOptions sizeOption_;

	void HandleNewGameMenuOpen();

	void UpdateSizeLabel();

	void StartGame();

public:
	virtual void HandleInitialize() override;

	void OpenNewGameMenu();

	void ScrollLeftwards();

	void ScrollRightwards();

	void GenerateWorld();

	void FinishGeneration();
};

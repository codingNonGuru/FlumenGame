#pragma once

#include "FlumenEngine/Interface/Element.hpp"

class Filter;

class MainMenu : public Element
{
protected:
	Filter* saturateFilter_;

	Filter* blurFilter_;

	Element* newGameMenu_;

	virtual void HandleInitialize();

	virtual void HandleOpen();

	virtual void HandleClose();

public:
	void OpenNewGameMenu();
};

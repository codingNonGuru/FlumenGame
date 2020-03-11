#pragma once

#include "FlumenEngine/Core/State.hpp"

class Element;
class Scene;

class PregameState : public State
{
private:
	Scene* pregameScene_;

	Element* mainMenu_;

	Element* newWorldMenu_;

	Element* worldPreviewPanel_;

	float mainMenuOpenDelay_;

protected:
	virtual void HandleEnter() override;

	virtual void HandleExit() override;

public:
	PregameState();
};

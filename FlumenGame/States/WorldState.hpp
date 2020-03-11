#pragma once

#include "FlumenEngine/Core/State.hpp"

class Scene;
class Element;

class WorldState : public State
{
private:
	Scene* worldScene_;

	Element* topPanel_;

	Element* topBar_;

	void HandleSceneEnable();

protected:
	virtual void HandleEnter() override;

	virtual void HandleExit() override;

public:
	WorldState();
};

#pragma once

#include "FlumenCore/Delegate/Delegate.hpp"

class Game
{
private:
	static Game* Register();

	static Game* instance_;

public:
	static Delegate OnStartGame_;

	static void Initialize();

	static void StartGame();

	static void HandleGameLoopStart();
};

#include "pch.h"
#include "Game.h"
#include "Engine.h"

void Game::BeginPlay(const WindowInfo& info)
{
	GEngine->Init(info);
}

void Game::Tick()
{
	GEngine->Render();
}

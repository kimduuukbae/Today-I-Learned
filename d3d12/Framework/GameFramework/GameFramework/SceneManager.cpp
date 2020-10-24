#include "stdafx.h"
#include "SceneManager.h"
#include "GameScene.h"

using namespace std;

void SceneManager::Init()
{
	Commit<GameScene>("GameScene"s);
	mainScene->Init();
}

void SceneManager::Draw()
{
	mainScene->Draw();
}

void SceneManager::Update(const GameTimer& gt)
{
	mainScene->Update(gt);
}
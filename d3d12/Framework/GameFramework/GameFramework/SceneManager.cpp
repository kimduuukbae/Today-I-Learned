#include "stdafx.h"
#include "SceneManager.h"
#include "Scene.h"

void SceneManager::Init()
{
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
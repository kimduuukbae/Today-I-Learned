#include "stdafx.h"
#include "SceneManager.h"
#include "GameScene.h"
#include "MeshManager.h"
#include "ResourceManager.h"

using namespace std;

SceneManager::~SceneManager()
{
	meshManager->Release();
	resourceManager->Release();
	scenes.clear();
}

void SceneManager::Init()
{
	meshManager = MeshManager::GetInstance();
	resourceManager = ResourceManager::GetInstance();

	meshManager->Init();
	resourceManager->Init();

	Commit<GameScene>("GameScene"s);
	mainScene->Init();
}

void SceneManager::Draw(ID3D12GraphicsCommandList* cmdList)
{
	resourceManager->BindingResource(cmdList);
	mainScene->Draw(cmdList);
}

void SceneManager::Update(const GameTimer& gt)
{
	mainScene->Update(gt);
}

void SceneManager::ClearUploadBuffer()
{
	meshManager->ClearUploadBuffer();
}

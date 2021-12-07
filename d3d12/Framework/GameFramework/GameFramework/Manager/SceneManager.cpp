#include "stdafx.h"
#include "SceneManager.h"
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
	
	mainScene->Init();
	resourceManager->Init();
}

void SceneManager::Draw(ID3D12GraphicsCommandList* cmdList)
{
	resourceManager->BindingResource(cmdList);
	mainScene->Draw(cmdList);
}

void SceneManager::PreProcessing(ID3D12GraphicsCommandList* cmdList)
{
	resourceManager->PreProcessing(cmdList);

	for (auto& elem : mainScene->objects[0])
		elem->Draw(cmdList);
	for (auto& elem : mainScene->objects[1])
		elem->Draw(cmdList);
}

void SceneManager::Update(const GameTimer& gt)
{
	mainScene->Update(gt);
}

void SceneManager::ClearUploadBuffer()
{
	meshManager->ReleaseUploadBuffer();
	resourceManager->ReleaseUploadBuffer();
}

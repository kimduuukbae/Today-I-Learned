#include "stdafx.h"
#include "Scene.h"
#include "CameraComponent.h"

void Scene::SetMainCamera(CameraComponent* target)
{
	mainCam = target;
}

void Scene::Draw(ID3D12GraphicsCommandList* cmdList)
{
	for (auto& it : objects)
		it->Draw(cmdList);
}

void Scene::Update(const GameTimer& gt)
{

}

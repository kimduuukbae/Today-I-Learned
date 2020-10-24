#include "stdafx.h"
#include "Scene.h"
#include "CameraComponent.h"

void Scene::SetMainCamera(CameraComponent* target)
{
	mainCam = target;
}

void Scene::Draw()
{

}

void Scene::Update(const GameTimer& gt)
{

}

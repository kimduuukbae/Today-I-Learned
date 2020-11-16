#include "stdafx.h"
#include "Scene.h"
#include "CameraComponent.h"

void Scene::Draw(ID3D12GraphicsCommandList* cmdList)
{
	for (auto& container : objects) {
		for (auto& elem : container.second)
			elem->Draw(cmdList);
	}
}

void Scene::Update(const GameTimer& gt)
{
	for (auto& container : objects) {
		for (auto& elem : container.second)
			elem->Update(gt);
	}
}


Object* Scene::FindObject(const std::string_view& name)
{
	for (auto& container : objects) {
		for (auto& elem : container.second) {
			if (elem->GetName() == name)
				return elem.get();
		}
	}
	return nullptr;
}

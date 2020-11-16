#include "stdafx.h"
#include "Scene.h"
#include "CameraComponent.h"
#include "CollisionComponent.h"

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
		auto& cont{ container.second };
		for (std::size_t i = 0; i < cont.size(); ++i)
			cont[i]->Update(gt);
	}

	for (size_t i = 0; i < collisionObjects.size(); ++i)
		for (size_t j = i + 1; j < collisionObjects.size(); ++j)
			collisionObjects[i].get().DoOverlap(collisionObjects[j]);
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

void Scene::AddCollision(CollisionComponent& collision)
{
	collisionObjects.emplace_back(collision);
}

void Scene::RemoveColliision(CollisionComponent& collision)
{
	const auto it{ std::find_if(
		collisionObjects.crbegin(),
		collisionObjects.crend(),
		[&](const CollisionComponent& other)
		{
			return &collision == &other;
		}) };

	if (it != collisionObjects.crend())
		collisionObjects.erase(it.base() - 1);
}

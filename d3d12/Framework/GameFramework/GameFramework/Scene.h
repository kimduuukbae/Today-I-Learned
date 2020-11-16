#pragma once
#include "Object.h"

class CameraComponent;
class SceneManager;
class CollisionComponent;

class Scene
{
public:
	Scene() = default;
	virtual ~Scene() {}

	template <typename T>
	T* SpawnObject(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& rot = DirectX::XMFLOAT3{})
	{
		auto p{ std::make_unique<T>() };
		p->curScene = this;
		T* ptr{ p.get() };
		ptr->Init();
		ptr->GetTransform()->SetTransform(pos, rot);

		uint32_t layer{ p->GetLayer() };
		objects[layer].push_back(std::move(p));

		return ptr;
	}

	template <typename T>
	T* SpawnObject()
	{
		auto p{ std::make_unique<T>() };
		p->curScene = this;
		T* ptr{ p.get() };
		ptr->Init();

		uint32_t layer{ p->GetLayer() };
		objects[layer].push_back(std::move(p));
		return ptr;
	}

	Object* FindObject(const std::string_view& name);

	void AddCollision(CollisionComponent& collision);
	void RemoveColliision(CollisionComponent& collision);

protected:
	virtual void Init() = 0;

private:
	friend SceneManager;

	virtual void Draw(ID3D12GraphicsCommandList* cmdList);
	void Update(const GameTimer& gt);

	std::map<uint32_t, std::vector<std::unique_ptr<Object>>> objects;
	std::vector<std::reference_wrapper<CollisionComponent>> collisionObjects;
};


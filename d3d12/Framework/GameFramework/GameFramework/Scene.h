#pragma once
#include "Object.h"

class CameraComponent;
class SceneManager;

class Scene
{
public:
	Scene() = default;
	virtual ~Scene() {}

	template <typename T>
	T* SpawnObject(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& rot)
	{
		auto p{ std::make_unique<T>() };
		uint32_t layer{ p->GetLayer() };

		objects[layer].push_back(std::move(p));

		T* tar{ static_cast<T*>(objects[layer].back().get()) };
		tar->Init();
		tar->GetTransform()->SetTransform(pos, rot);

		return tar;
	}

protected:
	virtual void Init() = 0;

private:
	friend SceneManager;

	virtual void Draw(ID3D12GraphicsCommandList* cmdList);
	void Update(const GameTimer& gt);

	std::unordered_map<int, std::vector<std::unique_ptr<Object>>> objects;
};


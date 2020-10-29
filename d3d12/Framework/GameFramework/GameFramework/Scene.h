#pragma once
#include "Object.h"

class CameraComponent;
class SceneManager;

class Scene
{
public:
	Scene() = default;
	virtual ~Scene() {}
	void SetMainCamera(CameraComponent* target);

	template <typename T>
	T* SpawnObject(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& rot)
	{
		this->objects.push_back(std::make_unique<T>());
		T* tar{ static_cast<T*>(this->objects.back().get()) };
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

	CameraComponent* mainCam{ nullptr };
	std::vector<std::unique_ptr<Object>> objects;
};


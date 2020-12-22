#pragma once

#include "UpdateComponent.h"
#include "TransformComponent.h"

class Scene;

class Object
{
public:
	Object();
	virtual ~Object();

	void SetTransform(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& rot);
	void SetPosition(const DirectX::XMFLOAT3& pos);
	void SetRotation(const DirectX::XMFLOAT3& rot);
	TransformComponent* GetTransform();
	template <typename T>
	T* AddComponent()
	{
		auto p{ std::make_unique<T>() };
		p->SetOwner(this);
		components.push_back(std::move(p));
		return static_cast<T*>(components.back().get());
	}

	template <typename T> requires std::is_base_of_v<UpdateComponent, T>
	T* AddComponent()
	{
		auto p{ std::make_unique<T>() };
		p->SetOwner(this);
		updateComponents.push_back(std::move(p));
		return static_cast<T*>(updateComponents.back().get());
	}

	template <typename T>
	T* GetComponent()
	{
		for (auto& it : components) {
			if (auto p = dynamic_cast<T*>(it.get()); p)
				return p;
		}

		for (auto& it : updateComponents) {
			if (auto p = dynamic_cast<T*>(it.get()); p)
				return p;
		}

		return nullptr;
	}

	virtual void Init() = 0;
	virtual void Update(const GameTimer& gt);
	virtual void Draw(ID3D12GraphicsCommandList* cmdList) = 0;

	void SetLayer(uint32_t layer);
	uint32_t GetLayer();

	Scene* GetScene();

	void SetName(std::string&& name);
	const std::string& GetName();

	void Activate();
	void DeActivate();
	bool IsActive() const;

	void Destroy();
protected:
	using Super = Object;

private:
	friend Scene;

	TransformComponent* transform;
	std::vector<std::unique_ptr<UpdateComponent>> updateComponents;
	std::vector<std::unique_ptr<IComponent>> components;

	uint32_t layer{ 0 };

	Scene* curScene;

	std::string name;
	bool isActive{ true };

	bool deleteFlag_{ false };
};


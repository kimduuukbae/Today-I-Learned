#pragma once

#include "IComponent.h"
#include "TransformComponent.h"

class Scene;

class Object
{
public:
	Object();
	virtual ~Object() = default;

	void SetTransform(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& rot);
	void SetPosition(const DirectX::XMFLOAT3& pos);
	void SetRotation(const DirectX::XMFLOAT3& rot);
	TransformComponent* GetTransform();
	template <typename T>
	std::unique_ptr<T> AddComponent()
	{
		return std::make_unique<T>();
	}

	template <typename T> requires std::is_base_of_v<IComponent, T>
	T* AddComponent()
	{
		auto p{ std::make_unique<T>() };
		p->SetOwner(this);
		components.push_back(std::move(p));
		return static_cast<T*>(components.back().get());
	}

	template <typename T>
	T* GetComponent()
	{
		for (auto& it : components)
			if (typeid(it) == T)
				return it;
		return nullptr;
	}

	virtual void Init() = 0;
	virtual void Update(const GameTimer& gt);
	virtual void Draw(ID3D12GraphicsCommandList* cmdList) = 0;

protected:
	using Super = Object;

private:
	friend Scene;

	std::unique_ptr<TransformComponent> transform;
	std::vector<std::unique_ptr<IComponent>> components;
};


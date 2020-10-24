#pragma once

class TransformComponent;

class Object
{
public:
	Object() = default;
	~Object() = default;
	Object(const Object& rhs) = default;
	Object(Object&&) = default;

	void SetTransform(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& rot);
	void SetPosition(const DirectX::XMFLOAT3& pos);
	void SetRotation(const DirectX::XMFLOAT3& rot);

private:
	friend class Scene;

	TransformComponent* transform{ nullptr };
};


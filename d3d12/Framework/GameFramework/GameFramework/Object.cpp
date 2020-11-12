#include "stdafx.h"
#include "Object.h"

Object::Object()
{
	transform = AddComponent<TransformComponent>();
}

Object::~Object()
{
}

void Object::SetTransform(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& rot)
{
	transform->SetTransform(pos, rot);
}

void Object::SetPosition(const DirectX::XMFLOAT3& pos)
{
	transform->SetPosition(pos);
}

void Object::SetRotation(const DirectX::XMFLOAT3& rot)
{
	transform->SetRotation(rot);
}

TransformComponent* Object::GetTransform()
{
	return transform.get();
}

void Object::Update(const GameTimer& gt)
{
	for (auto& it : components)
		it->Update(gt);
}

void Object::SetLayer(uint32_t layer)
{
	this->layer = layer;
}

uint32_t Object::GetLayer()
{
	return layer;
}

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
	return transform;
}

void Object::Update(const GameTimer& gt)
{
	for (auto& it : updateComponents) 
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

Scene* Object::GetScene()
{
	return curScene;
}

void Object::SetName(std::string&& name)
{
	this->name = std::move(name);
}

const std::string& Object::GetName()
{
	return name;
}

void Object::Activate()
{
	isActive = true;
}

void Object::DeActivate()
{
	isActive = false;
}

bool Object::IsActive() const
{
	return isActive;
}

void Object::Destroy()
{
	deleteFlag_ = true;
}

#include "stdafx.h"
#include "Object.h"
#include "TransformComponent.h"

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
